#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "Defines.h"
#include "AABB.h"
#include "Mesh.h"

Mesh AABB::box = Mesh();

AABB::AABB()
{
	m[0] = glm::vec3(std::numeric_limits<float>::infinity());
	m[1] = glm::vec3(-std::numeric_limits<float>::infinity());
	center = glm::vec3(NAN);
}
AABB::AABB(glm::vec3 min, glm::vec3 max)
{
	m[0] = min;
	m[1] = max;
	center = (m[0] + m[1]) / 2.0f;
}

void AABB::Draw(cCamera& cam, ShaderProgram& prog, glm::mat4 model) const
{
	if (!box.loaded)
	{
		box.Load("../data/Cube.obj");
	}

	glm::vec3 center = (m[0] + m[1]) / 2.0f;
	model = glm::translate(model, center);


	glm::vec3 size = m[1] - m[0];
	model = glm::scale(model, size);

	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function

	////Uncomment for wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);
	AABB::box.Draw(cam, prog, model);
	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	model = glm::mat4(1.0);
	model = glm::translate(model, m[0]);
	model = glm::scale(model, glm::vec3(0.1));
	AABB::box.Draw(cam, prog, model);

	model = glm::mat4(1.0);
	model = glm::translate(model, m[1]);
	model = glm::scale(model, glm::vec3( std::min((size.x+size.y+size.z)/3 * 0.1, 0.1)) );
	AABB::box.Draw(cam, prog, model);


	glDisable(GL_BLEND); //Enable blending.
}


glm::vec3 AABB::getCorner(short i)
{
	short a = 0b001 & i;
	short b = (0b010 & i) >> 1;
	short c = (0b100 & i) >> 2;
	std::cout << "Corner: " << m[c].x << " " << m[b].y << " " << m[a].z << std::endl;
	return glm::vec3(m[c].x, m[b].y, m[a].z);
}


short AABB::checkFrustum(Frustum& f, const short in_mask, short& out_mask)
{
	float m_prod, n_prod; short i, k = 1 << lastFrustumPlane, result = INSIDE;

	glm::vec4& sp = f.planes[lastFrustumPlane];

#ifdef MASKING
	out_mask = 0;
#endif

#ifdef COHERENCY
	//Check the plane that failed the test last time (if in mask)
	if (k & in_mask)
	{
		m_prod = (sp.x * m[sp.x < 0].x) + (sp.y * m[sp.y < 0].y) + (sp.z * m[sp.z < 0].z);
		if (m_prod > -sp.w) return OUTSIDE;

		n_prod = (sp.x * m[sp.x > 0].x) + (sp.y * m[sp.y > 0].y) + (sp.z * m[sp.z > 0].z);
		if (n_prod > -sp.w)
		{
		#ifdef MASKING
			out_mask |= k;
		#endif
			result = INTERSECTS;
		}
	}
#endif
	//Check the rest, skipping the last used plane and planes not in the mask
	for (i = 0, k = 1; k <= in_mask; i++, k += k)
		if (
#ifdef COHERENCY
			(i != lastFrustumPlane) &&
#endif
#ifdef MASKING
			(k & in_mask) &&
#endif
			true)
		{
			glm::vec4& p = f.planes[i];

			m_prod = (p.x * m[p.x < 0].x) + (p.y * m[p.y < 0].y) + (p.z * m[p.z < 0].z);
			if (m_prod > -p.w)
			{
				lastFrustumPlane = i;
				return OUTSIDE;
			}

			n_prod = (p.x * m[p.x > 0].x) + (p.y * m[p.y > 0].y) + (p.z * m[p.z > 0].z);
			if (n_prod > -p.w)
			{
#ifdef MASKING
				out_mask |= k;
#endif
				result = INTERSECTS;
			}
		}

	return result;
}

void AABB::insert(glm::vec3 point)
{
	for (int i = 0; i < 3; i++)
	{
		m[0][i] = std::min(point[i], m[0][i]);
		m[1][i] = std::max(point[i], m[1][i]);
	}
}
