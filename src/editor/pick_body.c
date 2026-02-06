#include "pick_body.h"

#include <float.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#include "engine/engine.h"
#include "camera/camera.h"

static void build_mouse_ray(const CameraState* cam, Vector3* out_origin, Vector3* out_dir)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    Vector2 mouse = GetMousePosition();

    // Normalized Device Coordinates (-1..1)
    float x = (2.0f * mouse.x) / w - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / h;

    float aspect = (float)w / (float)h;
    float fov_rad = cam->camera.fovy * DEG2RAD;

    float tan_fov = tanf(fov_rad * 0.5f);

    // Camera basis
    Vector3 forward = camera_forward(cam);
    Vector3 right   = Vector3Normalize(Vector3CrossProduct(forward, cam->camera.up));
    Vector3 up      = Vector3Normalize(Vector3CrossProduct(right, forward));

    Vector3 dir = Vector3Add(forward, Vector3Add(Vector3Scale(right, x * aspect * tan_fov),Vector3Scale(up, y * tan_fov)));

    *out_origin = cam->camera.position;
    *out_dir    = Vector3Normalize(dir);
}

static bool ray_sphere(Vector3 ro, Vector3 rd, Vector3 center, float radius, float* t_out)
{
    Vector3 oc = Vector3Subtract(ro, center);

    float b = Vector3DotProduct(oc, rd);
    float c = Vector3DotProduct(oc, oc) - radius * radius;
    float h = b*b - c;

    if (h < 0.0f) return false;

    h = sqrtf(h);
    float t = -b - h;
    if (t < 0.0f) t = -b + h;
    if (t < 0.0f) return false;

    *t_out = t;
    return true;
}

WorldID pick_body(const World* world)
{
    const CameraState* cam = &engine()->camera;

    Vector3 ray_origin, ray_dir;
    build_mouse_ray(cam, &ray_origin, &ray_dir);

    float best_t = FLT_MAX;
    WorldID best = WORLD_ID_INVALID;

    Body* body;
    world_foreach_body(world, body) {
        float t;
        if (ray_sphere(ray_origin, ray_dir, body->position, body->render.radius, &t)) {
            if (t < best_t) {
                best_t = t;
                best   = body->id;
            }
        }
    }

    return best;
}
