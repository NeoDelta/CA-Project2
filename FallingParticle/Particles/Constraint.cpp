#include "Constraint.h"

Constraint::Constraint(Particle& par1, Particle& par2, float elast, float damp)
{
	p1 = &par1;
	p2 = &par2;
	glm::vec3 vec = p1->getCurrentPosition() - p2->getCurrentPosition();
	rest_distance = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	std::cout << rest_distance << std::endl;
	elasticity = elast;
	damping = damp;
}

/* This is one of the important methods, where a single constraint between two particles p1 and p2 is solved
the method is called by Cloth.time_step() many times per frame*/
void Constraint::satisfyConstraint()
{
	glm::vec3 p1_to_p2 = p1->getCurrentPosition() - p2->getCurrentPosition(); // vector from p1 to p2
	float current_distance = sqrt(p1_to_p2.x * p1_to_p2.x + p1_to_p2.y * p1_to_p2.y + p1_to_p2.z * p1_to_p2.z); // current distance between p1 and p2
	p1_to_p2 = p1_to_p2 / current_distance;
	//glm::vec3 correctionVector = p1_to_p2*(1 - rest_distance / current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
	glm::vec3 correctionVector = p1_to_p2*(current_distance - rest_distance - rest_distance*0.3f);
	glm::vec3 correctionVectorHalf = correctionVector * 0.5f * 0.1f; // Lets make it half that length, so that we can move BOTH p1 and p2.
	//if (!p1->isFixed()) std::cout << "CVH: " << correctionVectorHalf.x << ", " << correctionVectorHalf.y << ", " << correctionVectorHalf.z << std::endl;
	if (!p1->isFixed()) p1->setPosition(p1->getCurrentPosition() - correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
	if (!p2->isFixed()) p2->setPosition(p2->getCurrentPosition() + correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	
}

void Constraint::calculateStringForces()
{

	glm::vec3 pos1 = p1->getCurrentPosition();
	glm::vec3 pos2 = p2->getCurrentPosition();
	glm::vec3 v1 = p1->getVelocity();
	glm::vec3 v2 = p2->getVelocity();
	float l = (pos2 - pos1).length();
	glm::vec3 n = (pos2 - pos1) / l;
	l = sqrt((pos2 - pos1).x * (pos2 - pos1).x + (pos2 - pos1).y * (pos2 - pos1).y + (pos2 - pos1).z * (pos2 - pos1).z);

	float fe = elasticity * (l - rest_distance);
	glm::vec3 fd = damping * (v2 - v1) * n;
	glm::vec3 f = (fe + fd) * n;

	p1->addStringfForce(f);
	p2->addStringfForce(-f);
}