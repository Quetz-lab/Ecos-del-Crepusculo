#include "Player.h"

using namespace Quetz_LabEDC;

void Quetz_LabEDC::Player::update()
{
	Vector2 newpos;
	newpos = position;
	if (IsKeyDown(KEY_A))
	{
		newpos.x -= speed * GetFrameTime();
		animData.direction = ANIM_LEFT;
	}
	if (IsKeyDown(KEY_D))
	{
		newpos.x += speed * GetFrameTime();
		animData.direction = ANIM_RIGHT;
	}
	if (IsKeyDown(KEY_W))
	{
		newpos.y -= speed * GetFrameTime();
		animData.direction = ANIM_UP;
	}
	if (IsKeyDown(KEY_S))
	{
		newpos.y += speed * GetFrameTime();
		animData.direction = ANIM_DOWN;
	}

	if (IsGamepadAvailable(0)) {
		float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
		float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

		if (fabs(axisX) > 0.2f) { // Umbral para evitar movimiento no intencionado
			newpos.x += axisX * speed * GetFrameTime();
			animData.direction = (axisX < 0) ? ANIM_LEFT : ANIM_RIGHT;
		}

		if (fabs(axisY) > 0.2f) {
			newpos.y += axisY * speed * GetFrameTime();
			animData.direction = (axisY < 0) ? ANIM_UP : ANIM_DOWN;
		}
	}


	if (!Level::getInstance().CheckCollision(newpos))
	{
		position = newpos; //solo mover si no hay colision
	}

	////calcular el frame de la animacion
	animData.frameCounter++;
	if (animData.frameCounter > animData.frameSpeed)
	{
		animData.frameCounter = 0;
		animData.currentFrame++;

		if (animData.currentFrame >= animData.maxFrames)
			animData.currentFrame = 0;

		//std::cout << "Frame: " << animData.currentFrame << std::endl;
	}

	//si tiene arma, hacer que se mueva con el jugador
	if (weapon)
	{
		Weapon* w = dynamic_cast<Weapon*>(weapon);
		w->position = Vector2Add(position, w->offset);
	}

	//detectar colisiones con armas tiradas en el suelo
	for (GameObject* obj : GameObject::gameObjects)
	{
		Weapon* w = dynamic_cast<Weapon*>(obj);
		if (w &&
			w->owner == nullptr &&
			CheckCollisionRecs({ position.x, position.y, animData.spriteWidth, animData.spriteHeight }, {w->position.x, w->position.y,64,64}))
		{
			//std::cout << "Colision con arma: " << w->name << std::endl;
			//SetWeapon(w); //cambiar el arma del jugador
			shouldPromptForWeapon = true; //mostrar mensaje de recoger arma

			if (IsKeyPressed(KEY_F)) //si se presiona F
			{
				SetWeapon(w); //cambiar el arma del jugador
				shouldPromptForWeapon = false;
			}
			break;
		}
		else
			shouldPromptForWeapon = false; //no hay arma cerca
	}
}
	




void Quetz_LabEDC::Player::draw()
{
	Rectangle r = { animData.spriteWidth * animData.currentFrame ,
					animData.spriteHeight * animData.direction,
					animData.spriteWidth,
					animData.spriteHeight };
	DrawTextureRec(texture, r, position, WHITE);

	if (shouldPromptForWeapon)
		DrawText(weaponPrompt, 20, GetScreenHeight() - 40, 20, YELLOW);
	//DrawTexture(texture, position.x, position.y, WHITE);
}

void Player::Fire()
{
	//checar qe el player tiene un IAttacker y 
	//entonces llamar a su fire
}

IAttacker* Player::SetWeapon(IAttacker* newWeapon)
{
	if (newWeapon)
	{
		weapon = newWeapon;
		Weapon* w = dynamic_cast<Weapon*>(weapon);
		if (w)
		{
			w->owner = this; //asignar el owner al arma
			std::cout << "cambiando arma a " << w->name << std::endl;
		}

		//notificar a los sidekicks
		//for (int i = 0; i < 3; i++)
		//{
		//	if (sidekicks[i])
		//	{
		//		//sidekicks[i]->SetWeapon(newWeapon);
		//	}
		//}

		return weapon;
	}
	return nullptr;
}

//void Player::SetSidekick(Sidekick* newSidekick, int index)
//{
//	//if (index >= 0 && index < 3)
//	//{
//	//	sidekicks[index] = newSidekick;
//	//}
//	//else
//	//{
//	//	std::cout << "Indice de sidekick invalido" << std::endl;
//	//}
//}