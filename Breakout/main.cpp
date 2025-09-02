#include "headers/MyWindow.h"
#include "headers/Renderer.h"
#include "headers/InputManager.h"
#include "headers/PhysicsBody.h" 
#include "SoundManager.h"
#include <iostream>
#include <chrono>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

// TO REMOVE --------------------------------------------------------------------
// Simple circle collision (very small, easy to understand)
void SimpleResolveCircleCollision(SpriteInstance& A_spr, PhysicsBody& A_body,
                                  SpriteInstance& B_spr, PhysicsBody& B_body,
                                  float restitution = 0.6f) // 0..1, 1 = perfectly elastic
{
    // centers (assumes sprite.position is the center)
    float ax = A_spr.position.x;
    float ay = A_spr.position.y;
    float bx = B_spr.position.x;
    float by = B_spr.position.y;

    float dx = bx - ax;
    float dy = by - ay;
    float dist2 = dx*dx + dy*dy;
    float rSum = A_body.radius + B_body.radius;
    if (dist2 >= rSum * rSum) return; // no collision

    float dist = sqrtf(dist2);
    // avoid division by zero
    D3DXVECTOR3 normal;
    if (dist > 1e-6f) {
        normal = D3DXVECTOR3(dx / dist, dy / dist, 0.0f); // this get unit vector of normal
    } else {
        // perfectly overlapping; choose arbitrary normal
        normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
        dist = 0.0f;
    }

    // penetration and simple positional correction (split by mass)
    float penetration = rSum - dist;
    float invMassA = (A_body.mass > 0.0f) ? 1.0f / A_body.mass : 0.0f;
    float invMassB = (B_body.mass > 0.0f) ? 1.0f / B_body.mass : 0.0f;
    float invSum = invMassA + invMassB;
    if (invSum > 0.0f) {
        // move them out so they no longer overlap
        D3DXVECTOR3 correction = D3DXVECTOR3(normal.x * (penetration / invSum),
                                             normal.y * (penetration / invSum),
                                             0.0f);
        // heavier object moves less
        A_spr.position.x -= correction.x * invMassA;
        A_spr.position.y -= correction.y * invMassA;
        B_spr.position.x += correction.x * invMassB;
        B_spr.position.y += correction.y * invMassB;
    }

    // relative velocity
    D3DXVECTOR3 relVel = D3DXVECTOR3(B_body.velocity.x - A_body.velocity.x,
                                     B_body.velocity.y - A_body.velocity.y, 0.0f);
    // dot product to project vector onto the normal
    float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y; // dot product between vector and normal,  v . n

    // if moving apart already, no impulse needed
    if (velAlongNormal > 0.0f) return;

    // compute impulse scalar (very simple)
    float e = restitution;
    float j = 0.0f;
    if (invSum > 0.0f) {
        j = -(1.0f + e) * velAlongNormal / invSum;
    }

    // apply impulse
    D3DXVECTOR3 impulse = D3DXVECTOR3(normal.x * j, normal.y * j, 0.0f);
    if (A_body.mass > 0.0f) {
        A_body.velocity.x -= impulse.x * invMassA;
        A_body.velocity.y -= impulse.y * invMassA;
    }
    if (B_body.mass > 0.0f) {
        B_body.velocity.x += impulse.x * invMassB;
        B_body.velocity.y += impulse.y * invMassB;
    }
}
// --------------------------------------------------------------------------

// Assets to be preload
std::vector<std::string> textureList = {
    //"assets/paddle.png",
    "assets/bg3.png",
    "assets/singlePaddle.png",
    "assets/ball.png",
};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, L"Breakout Game");
    Renderer renderer;
    InputManager inputManager;
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    if (!renderer.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight())) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return -1;
    }
    if (!renderer.LoadTexturesBatch(textureList)) {
        MessageBoxW(nullptr, L"Failed to load textures!", L"Error", MB_ICONERROR);
        return -1;
    }
    // override or load militia with logical size 128x192
    renderer.LoadTexture("assets/militia.png", 128, 192);

    if (!inputManager.Initialize(window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize input manager!", L"Error", MB_ICONERROR);
        return -1;
    }

    //// Background (render order 0 - renders first)
    //SpriteInstance background("assets/bg3.png", D3DXVECTOR3(0, 0, 0), 0);
    //renderer.AddRenderItem(background);

    // Paddle (render order 1 - renders on top of background)
    SpriteInstance paddle("assets/singlePaddle.png", D3DXVECTOR3(400, 500, 0), 1);
    renderer.AddRenderItem(paddle);

    SpriteInstance spaceship("assets/practical9.png", D3DXVECTOR3(200, 200, 0), 2, 2, 2, 2,0.1f, true, true);
    renderer.AddRenderItem(spaceship);
	SpriteInstance spaceship2("assets/practical9.png", D3DXVECTOR3(400, 200, 0), 2, 2, 2, 2,0.1f, true, true);
    spaceship2.SetState(1); // white spaceship
    renderer.AddRenderItem(spaceship2);

    // Ball (render order 2 - renders on top of everything)
    SpriteInstance ball("assets/ball.png", D3DXVECTOR3(500, 300, 0), 2);
    ball.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f); // Make ball smaller
    renderer.AddRenderItem(ball);

    SpriteInstance militia("assets/militia.png", D3DXVECTOR3(600, 400, 0), 2, 4, 4, 4, 0.1f, true, true);
    renderer.AddRenderItem(militia);

    // Game variables for ball movement
    float ballX = 500, ballY = 300;
    float ballSpeedX = 5.0f, ballSpeedY = 5.0f;
    D3DXVECTOR3 currentBallPos(ballX, ballY, 0); // Track current position

    // Spaceship variable
    PhysicsBody shipBody;
    shipBody.mass = 1.0f;
	// Spaceship 2 variable
    PhysicsBody shipBody2;
    shipBody2.mass = 1.0f;

    // deltaTime variable
    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();

    SoundManager* soundManager = new SoundManager();
    soundManager->InitializeAudio();
    soundManager->LoadSounds();
    soundManager->PlaySound1();
    soundManager->PlaySoundTrack();

    while (window.ProcessMessages()) {
        // get delta time
		auto now = clock::now();
		std::chrono::duration<float> elapsed = now - last;
		last = now;
		float deltaTime = elapsed.count(); // seconds since last frame	

        // Game update and render calls here
        inputManager.Update();

        int playerState = -1;
		bool moved = false;

		if (inputManager.IsKeyDown(DIK_UP)) {
			playerState = 3;
			militia.position.y -= 5.0f;
			moved = true;
		} else if (inputManager.IsKeyDown(DIK_DOWN)) {
			playerState = 0;
			militia.position.y += 5.0f;
			moved = true;
		} else if (inputManager.IsKeyDown(DIK_LEFT)) {
			playerState = 1;
			militia.position.x -= 5.0f;
			moved = true;
        }
        else if (inputManager.IsKeyDown(DIK_RIGHT)) {
            playerState = 2;
            militia.position.x += 5.0f;
            moved = true;
        }

        if (inputManager.IsKeyDown(DIK_A)) {
            spaceship.rotation -= 0.1f;
        } else if (inputManager.IsKeyDown(DIK_D)) {
            spaceship.rotation += 0.1f;
        }else if (inputManager.IsKeyDown(DIK_W)) {
            D3DXVECTOR3 thrust;
            float thrustPower = 1000.0f;
            thrust.x = sinf(spaceship.rotation) * thrustPower;
            thrust.y = -cosf(spaceship.rotation) * thrustPower;
            shipBody.ApplyForce(thrust);
        }
		if (inputManager.IsKeyDown(DIK_J)) {
            spaceship2.rotation -= 0.1f;
        } else if (inputManager.IsKeyDown(DIK_L)) {
            spaceship2.rotation += 0.1f;
        }else if (inputManager.IsKeyDown(DIK_I)) {
            D3DXVECTOR3 thrust;
            float thrustPower = 1000.0f;
            thrust.x = sinf(spaceship2.rotation) * thrustPower;
            thrust.y = -cosf(spaceship2.rotation) * thrustPower;
            shipBody2.ApplyForce(thrust);
        }

        shipBody.UpdatePhysics(deltaTime);
        spaceship.position.x += shipBody.velocity.x * deltaTime;
        spaceship.position.y += shipBody.velocity.y * deltaTime;

        shipBody2.UpdatePhysics(deltaTime);
        spaceship2.position.x += shipBody2.velocity.x * deltaTime;
        spaceship2.position.y += shipBody2.velocity.y * deltaTime;
        
        SimpleResolveCircleCollision(spaceship, shipBody, spaceship2, shipBody2, 0.0f);

        if (spaceship.position.x >= SCREEN_WIDTH || spaceship.position.x <= 0) {
            shipBody.velocity.x = -shipBody.velocity.x;
        }
        if (spaceship.position.y >= SCREEN_HEIGHT || spaceship.position.y <= 0) {
            shipBody.velocity.y = -shipBody.velocity.y;
        }
		if (spaceship2.position.x >= SCREEN_WIDTH || spaceship2.position.x <= 0) {
            shipBody2.velocity.x = -shipBody2.velocity.x;
        }
        if (spaceship2.position.y >= SCREEN_HEIGHT || spaceship2.position.y <= 0) {
            shipBody2.velocity.y = -shipBody2.velocity.y;
        }

		// Only change state if different
		static int lastMilitiaState = militia.state;
		if (playerState != -1 && playerState != lastMilitiaState) {
			militia.SetState(playerState, true);
			militia.playing = true;
			lastMilitiaState = playerState;
		}
        renderer.UpdateRenderItem(militia);
        renderer.UpdateRenderItem(spaceship);
        renderer.UpdateRenderItem(spaceship2);

        //// Update ball position
        //currentBallPos = D3DXVECTOR3(ballX, ballY, 0);
        //ballX += ballSpeedX;
        //ballY += ballSpeedY;

        //// Simple bounce logic
        //if (ballX <= 0 || ballX >= SCREEN_WIDTH - 32) ballSpeedX = -ballSpeedX;
        //if (ballY <= 0 || ballY >= SCREEN_HEIGHT - 32) ballSpeedY = -ballSpeedY;

        //// Update ball position in render queue
        //SpriteInstance updatedBall("assets/ball.png", D3DXVECTOR3(ballX, ballY, 0), 2);
        //updatedBall.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f);
        //renderer.UpdateRenderItem("assets/ball.png", currentBallPos, updatedBall);

        renderer.Update(deltaTime);
        renderer.Render();
        
    }

    return 0;
}