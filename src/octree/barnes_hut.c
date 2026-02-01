#include "octree.h"

static void bh_insert(OctreeNode* node, GravBodyView* body)
{
    Vector3 pos = *body->position;

    node->com = Vector3Add(Vector3Scale(node->com, node->mass), Vector3Scale(pos, body->mass));
    node->mass += body->mass;
    node->com = Vector3Scale(node->com, 1.0f / node->mass);

    octree_insert(node, body, bh_insert);
}

static void octree_apply_force(const OctreeNode* node, GravBodyView* bodies, int i, const OctreeConfig* cfg)
{
    if (!node || node->mass == 0.0f) return;
    if (node->body == &bodies[i]) return;

    Vector3 d = Vector3Subtract(node->com, *bodies[i].position);
    float dist2 = Vector3LengthSqr(d) + cfg->softening * cfg->softening;
    float dist = sqrtf(dist2);
    float size = node->half_size * 2.0f;

    if ((!node->children[0] && node->body) || (size / dist) < cfg->theta) {
        float inv_r3 = 1.0f / (dist2 * dist);
        Vector3 a = Vector3Scale(d, cfg->G * node->mass * inv_r3);
        *bodies[i].acceleration = Vector3Add(*bodies[i].acceleration, a);
        return;
    }

    for (int k = 0; k < 8; k++)
        octree_apply_force(node->children[k], bodies, i, cfg);
}

void bh_compute_gravity(GravBodyView* bodies, int count, const OctreeConfig* cfg)
{
    if (count == 0) return;

    Vector3 min = *bodies[0].position;
    Vector3 max = *bodies[0].position;

    for (int i = 1; i < count; i++) {
        min = Vector3Min(min, *bodies[i].position);
        max = Vector3Max(max, *bodies[i].position);
    }

    Vector3 center = Vector3Scale(Vector3Add(min, max), 0.5f);
    float half_size = 0.5f * fmaxf(fmaxf(max.x - min.x, max.y - min.y), max.z - min.z) + 1e-3f;

    OctreeNode* root = octree_create(center, half_size);
    for (int i = 0; i < count; i++) bh_insert(root, &bodies[i]);

    for (int i = 0; i < count; i++) octree_apply_force(root, bodies, i, cfg);

    octree_free(root);
}
