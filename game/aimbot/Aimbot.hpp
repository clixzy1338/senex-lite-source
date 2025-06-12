#pragma once
#include <cstdint>
#include "../../sdk-and-offsets/sdk.hpp"
#include "../../driver/comm.hpp"
#include "../../settings/settings.hpp"

uintptr_t Memoryaim = 0x520 + 0x8; //size of NetConnection (0x8)
bool memory_event(Vector3 newpos)
{
	write<Vector3>(cache::player_controller + Memoryaim, newpos); //write Vectors to control rotation
	return true;
}

void memoryaim(uintptr_t target_mesh)
{
	if (!target_mesh) return;
	if (!is_visible(target_mesh)) return;

	int bone_id = settings::aimbot::current_hitbox;
	Vector3 head3d = get_entity_bone(target_mesh, bone_id);

	if (settings::aimbot::bulletPrediction)
	{
		float projectileVelocity = read<float>(cache::current_weapon + 0x220c);
		float projectileGravity = read<float>(cache::current_weapon + 0x2210);

		if (projectileVelocity > 0)
		{
			float height = head3d.z - cache::local_camera.location.z;
			float depthPlayerTarget = sqrt(pow(head3d.y - cache::local_camera.location.y, 2) * pow(head3d.x - cache::local_camera.location.x, 2));
			float bulletDrop = getBulletDrop(height, depthPlayerTarget, projectileVelocity, projectileGravity);

			head3d.z += bulletDrop;
		}
	}

	Vector2 head2d = project_world_to_screen(head3d);
	Vector2 target{};

	if (head2d.x != 0)
	{
		if (head2d.x > settings::screen_center_x)
		{
			target.x = -(settings::screen_center_x - head2d.x);
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x > settings::screen_center_x * 2) target.x = 0;
		}
		if (head2d.x < settings::screen_center_x)
		{
			target.x = head2d.x - settings::screen_center_x;
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x < 0) target.x = 0;
		}
	}
	if (head2d.y != 0)
	{
		if (head2d.y > settings::screen_center_y)
		{
			target.y = -(settings::screen_center_y - head2d.y);
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y > settings::screen_center_y * 2) target.y = 0;
		}
		if (head2d.y < settings::screen_center_y)
		{
			target.y = head2d.y - settings::screen_center_y;
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y < 0) target.y = 0;
		}
	}
	Vector3 new_rotation = { -target.y / 5, target.x / 5, 0 };
	memory_event(new_rotation);
}


