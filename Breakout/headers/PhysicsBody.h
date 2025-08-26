#pragma once
#include <d3dx9.h>

struct PhysicsBody {
	float mass = 1.0f;
	D3DXVECTOR3 force = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 acceleration = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);

	// to remove
	float radius = 16.0f;
	// ------------

	void ApplyForce(const D3DXVECTOR3& f) {
		force += f;
	}

	void UpdatePhysics(float dt) {
		if (mass <= 0.0f) return; // avoid divide by zero
		acceleration = force / mass;
		velocity = velocity + acceleration * dt;
		force = D3DXVECTOR3(0, 0, 0); // reset force
		acceleration = D3DXVECTOR3(0, 0, 0); // reset accelaration
	}
};
