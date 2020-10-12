
#include <iostream>

#include "Defines.h"
#include "BVHNode.h"

BVHNode::BVHNode()
{
	type = TYPE_UNINITIALIZED; // Unitialized
}


BVHNode::BVHNode(std::vector<RenderObject>& objects) : BVHNode(new std::vector<RenderObject>(objects))
{
}

BVHNode::BVHNode(std::vector<RenderObject>* objects)
{
	//std::cout << "--Node " << this << " created" << std::endl;
	type = TYPE_TOSPLIT;
	if (objects->size() == 0)
	{
		type = TYPE_LEAF;
		delete objects;
		contents.object = nullptr;
		return;
	}
	if (objects->size() == 1)
	{
		type = TYPE_LEAF;
		RenderObject* r = new RenderObject((*objects)[0]);
		delete objects;
		contents.object = r;
		bounds = r->bounds;
		return;
	}

	contents.objectVec = objects;

	glm::vec3 min = (*contents.objectVec)[0].bounds.m[0];
	glm::vec3 max = (*contents.objectVec)[0].bounds.m[1];
	min_center = max_center  = (*contents.objectVec)[0].bounds.center;
	for (auto& object : *contents.objectVec)
	{
		for (int i = 0; i < 3; i++)
		{
			min[i] = ((object.bounds.m[0])[i] < min[i]) ? (object.bounds.m[0])[i] : min[i];
			min_center[i] = ((object.bounds.center)[i] < min_center[i]) ? (object.bounds.center)[i] : min_center[i];
			max[i] = ((object.bounds.m[1])[i] > max[i]) ? (object.bounds.m[1])[i] : max[i];
			max_center[i] = ((object.bounds.center)[i] > max_center[i]) ? (object.bounds.center)[i] : max_center[i];
		}
	}
	bounds = AABB(min, max);
	/*
	std::cout << std::endl;
	std::cout << "Created a node with " << objects->size() << " renderobjects and bounds of: " << std::endl
		<< "min=" << min.x << ":" << min.y << ":" << min.z << " max=" << max.x << ":" << max.y << ":" << max.z
		<< " on address " << objects << std::endl;
	*/
}

BVHNode::~BVHNode()
{
	//std::cout << "--Node " << this << " of type " << type << " destroyed" << std::endl;
	switch (type)
	{
	case TYPE_TOSPLIT:
		delete contents.objectVec;
		break;
	case TYPE_LEAF:
		delete contents.object;
		break;
	case TYPE_INNER:
		delete contents.children;
		break;
	default:
		std::cout << "Unitialized node is deleted, weird behavior." << std::endl;
		break;
	}

}

void BVHNode::MidpointSplit(unsigned int sizetarget)
{
	//std::cout << "Splitting type " << type << std::endl;
	if (sizetarget < 1)
	{
		throw "node size target too small!";
	}

	if (type != TYPE_TOSPLIT || (contents.objectVec)->size() < sizetarget)
	{
		//Node already split or smaller than target size, not doing anything
		return;
	}
	//SHOULDN'T split a node based on outer bounds but on center bounds

	glm::vec3 diff = glm::abs(max_center - min_center);

	short splitaxis = diff.x > diff.y ? (diff.x > diff.z ? 0 : 2) : (diff.y > diff.z ? 1 : 2);

	float midpoint = (min_center[splitaxis] + max_center[splitaxis]) / 2;

	std::vector<RenderObject>* leftchild = new std::vector<RenderObject>();
	std::vector<RenderObject>* rightchild = new std::vector<RenderObject>();

	for (long unsigned int i = 0; i < (contents.objectVec)->size(); i++)
	{
		if ((*contents.objectVec)[i].bounds.center[splitaxis] < midpoint)
			leftchild->push_back((*contents.objectVec)[i]);
		else
			rightchild->push_back((*contents.objectVec)[i]);
	}
	if (rightchild->empty())
	{
		rightchild->push_back(leftchild->back());
		leftchild->pop_back();
	}
	else if (leftchild->empty())
	{
		leftchild->push_back(rightchild->back());
		rightchild->pop_back();
	}
	delete contents.objectVec;
	contents.children = new std::array<BVHNode, 2>();
	
	//Overwrite the data directly, without the need of a copy function
	//We can do this because the default constructor doesn't call new
	new ((contents.children)->data()) BVHNode(leftchild);
	new ((contents.children)->data()+1) BVHNode(rightchild);

	type = 0;

	(*contents.children)[0].MidpointSplit();
	(*contents.children)[1].MidpointSplit();
}

void BVHNode::Draw(cCamera& cam, ShaderProgram& prog, Frustum *f, short mask)
{
	switch (type)
	{
	case 0:
		//Drawing inner node
		if (f == nullptr)
		{
			// recurse into children
			(*contents.children)[0].Draw(cam, prog, nullptr);
			(*contents.children)[1].Draw(cam, prog, nullptr);
		}
		else
		{
			// Test both AABBs with frustum
			for (BVHNode& childNode : *(contents.children))
			{
				short newmask;
				short state = childNode.bounds.checkFrustum(*f, mask, newmask);
				switch (state)
				{
				case INSIDE:
					// if INSIDE, draw with nullptr frustum
					childNode.Draw(cam, prog, nullptr);
					break;
				case INTERSECTS:
					// if INTERSECTS, recurse with f and new mask
#ifdef MASKING
					childNode.Draw(cam, prog, f, newmask);
#else
					childNode.Draw(cam, prog, f, 0b111111);
#endif
					break;
				case OUTSIDE:
					// if OUTSIDE, do nothing
					break;
				}

				//return;
			}

		}
		break;
	case 1:
		//Drawing leaf
		if (contents.object != nullptr)
			contents.object->Draw(cam, prog);
		break;
	case 2:
		//Drawing unsplit bvh node
		for (RenderObject& obj : (*contents.objectVec))
		{
			obj.Draw(cam, prog);
		}
	}
}


unsigned int BVHNode::CountCalls(Frustum *f, short mask)
{

	unsigned int retval = 0;
	switch (type)
	{
	case 0:
		//Drawing inner node
		if (f == nullptr)
		{
			// recurse into children
			retval += (*contents.children)[0].CountCalls(nullptr);
			retval += (*contents.children)[1].CountCalls(nullptr);
		}
		else
		{
			// Test both AABBs with frustum
			for (BVHNode& childNode : *(contents.children))
			{
				short newmask;
				short state = childNode.bounds.checkFrustum(*f, mask, newmask);
				switch (state)
				{
				case INSIDE:
					// if INSIDE, draw with nullptr frustum
					retval += childNode.CountCalls(nullptr);
					break;
				case INTERSECTS:
					// if INTERSECTS, recurse with f and new mask
				#ifdef MASKING
					retval += childNode.CountCalls(f, newmask);
				#else
					retval += childNode.CountCalls(f, 0b111111);
				#endif
					break;
				case OUTSIDE:
					// if OUTSIDE, do nothing
					break;
				}
			}

		}
		break;
	case 1:
		//Drawing leaf
		if (contents.object != nullptr)
			retval += 1;
		break;
	case 2:
		//Drawing unsplit bvh node
		for (long unsigned int i = 0; i < contents.objectVec->size(); i++)
		{
			retval += 1;
		}
	}
	return retval;
}



void BVHNode::DrawBounds(cCamera& cam, ShaderProgram& prog, int level)
{
	if (level == 0)
	{
		bounds.Draw(cam, prog);
		return;
	}
	else if (type == 0)
	{
		// Drawing inner node
		// recurse into children
		(*contents.children)[0].DrawBounds(cam, prog, level - 1);
		(*contents.children)[1].DrawBounds(cam, prog, level - 1);
	}
}