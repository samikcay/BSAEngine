using System;
using BSAEngine;

namespace Sandbox
{
    public class Player : Entity
    {
        private float Speed = 5.0f;

        protected override void OnCreate()
        {
            InternalCalls.NativeLog($"Player objesi olusturuldu! ID: {ID}", 1);
        }

        protected override void OnUpdate(float ts)
        {
            Vector3 position = Translation;
            bool moved = false;

            // Engine 'ts' süresini yolluyor, bunu hız ile carpip smooth hareket elde ediyoruz.
            float velocity = Speed * ts;

            if (Input.IsKeyDown(KeyCode.W))
            {
                position.Y += velocity;
                moved = true;
            }
            else if (Input.IsKeyDown(KeyCode.S))
            {
                position.Y -= velocity;
                moved = true;
            }

            if (Input.IsKeyDown(KeyCode.A))
            {
                position.X -= velocity;
                moved = true;
            }
            else if (Input.IsKeyDown(KeyCode.D))
            {
                position.X += velocity;
                moved = true;
            }

            if (moved)
            {
                Translation = position;
            }
        }
    }
}
