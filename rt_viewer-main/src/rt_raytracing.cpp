#include "rt_raytracing.h"
#include "rt_ray.h"
#include "rt_hitable.h"
#include "rt_sphere.h"
#include "rt_triangle.h"
#include "rt_box.h"
#include "rt_utils.h"


#include "cg_utils2.h"  // Used for OBJ-mesh loading
#include <stdlib.h>     // Needed for drand48()

namespace rt {

// Store scene (world) in a global variable for convenience
struct Scene {
    Sphere ground;
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;
    std::vector<Triangle> mesh;
    Box mesh_bbox;
} g_scene;

bool hit_world(const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    if (g_scene.ground.hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
    }
    for (int i = 0; i < g_scene.spheres.size(); ++i) {
        if (g_scene.spheres[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.boxes.size(); ++i) {
        if (g_scene.boxes[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.mesh.size(); ++i) {
        if (g_scene.mesh[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

// This function should be called recursively (inside the function) for
// bouncing rays when you compute the lighting for materials, like this
//
// if (hit_world(...)) {
//     ...
//     return color(rtx, r_bounce, max_bounces - 1);
// }
//
// See Chapter 7 in the "Ray Tracing in a Weekend" book
glm::vec3 color(RTContext &rtx, const Ray &r, int max_bounces)
{
    if (max_bounces < 0) return glm::vec3(0.0f);

    HitRecord rec;
    if (hit_world(r, 0.0f, 9999.0f, rec)) {
        rec.p = rec.p + (0.0001f * rec.normal);
        rec.normal = glm::normalize(rec.normal);  // Always normalise before use!
        if (rtx.show_normals) { return rec.normal * 0.5f + 0.5f; }

        // Implement lighting for materials here
        Ray scattered;
        if (rec.mat->scatter(r, rec, scattered))
            return rec.mat->color * color(rtx, scattered, max_bounces-1);
        return glm::vec3(0,0,0);

        //glm::vec3 collisionPoint = rec.p + (0.0001f * rec.normal);
        //Ray r_bounce(collisionPoint, random_on_hemisphere(rec.normal));
        //glm::vec3 newColor = color(rtx, r_bounce, max_bounces - 1);
        //std::shared_ptr<Material> getMat = rec.mat;
        //newColor *= getMat->color;

        //return 0.5f * newColor;
    }

    // If no hit, return sky color
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * rtx.ground_color + t * rtx.sky_color;
}

// MODIFY THIS FUNCTION!
void setupScene(RTContext &rtx,const char *filename)
{

    //Setup Materials
    auto metalMaterial = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8));
    auto defaultMaterial = std::make_shared<Lambertian>(glm::vec3(0.6, 0.0, 0.6));
    auto defaultMaterial2 = std::make_shared<Lambertian>(glm::vec3(0.2, 0.3, 0.5));

    g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, defaultMaterial2);
    g_scene.spheres = {
        Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, metalMaterial),
        Sphere(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, defaultMaterial),
        Sphere(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, defaultMaterial),
    };
    
    g_scene.boxes = {
        Box(glm::vec3(0.0f, -0.25f, 0.0f), glm::vec3(0.25f), g_scene.mesh),
        Box(glm::vec3(1.0f, -0.25f, 0.0f), glm::vec3(0.25f), g_scene.mesh),
        Box(glm::vec3(-1.0f, -0.25f, 0.0f), glm::vec3(0.25f), g_scene.mesh),
    };

    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    g_scene.mesh.clear();
    //g_scene.boxes.clear();
    auto matt = std::make_shared<Lambertian>(glm::vec3(0.2, 0.3, 0.5));
    for (int i = 0; i < mesh.indices.size(); i += 3) {
       int i0 = mesh.indices[i + 0];
       int i1 = mesh.indices[i + 1];
       int i2 = mesh.indices[i + 2];
       glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.135f, 0.0f);
       glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.135f, 0.0f);
       glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.135f, 0.0f);
       g_scene.mesh.push_back(Triangle(v0, v1, v2,matt));
     }
     
}



void updateScene(RTContext &rtx)
{

    std::shared_ptr<Material> sphereMaterial1;
    std::shared_ptr<Material> sphereMaterial2;
    std::shared_ptr<Material> sphereMaterial3;

    if (rtx.show_s1_metal) {
        sphereMaterial1 = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8));
    } else {
        sphereMaterial1 = std::make_shared<Lambertian>(glm::vec3(0.6, 0.0, 0.6));
    }

    if (rtx.show_s2_metal) {
        sphereMaterial2 = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8));
    } else {
        sphereMaterial2 = std::make_shared<Lambertian>(glm::vec3(0.6, 0.0, 0.6));
    }

    if (rtx.show_s3_metal) {
        sphereMaterial3 = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8));
    } else {
        sphereMaterial3 = std::make_shared<Lambertian>(glm::vec3(0.6, 0.0, 0.6));
    }

   
    auto defaultMaterial2 = std::make_shared<Lambertian>(glm::vec3(0.2, 0.3, 0.5));

    g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, defaultMaterial2);
    g_scene.spheres = { 
        Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, sphereMaterial1),
        Sphere(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, sphereMaterial2),
        Sphere(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, sphereMaterial3),
    };

    
}

// MODIFY THIS FUNCTION!
void updateLine(RTContext &rtx, int y)
{
    int nx = rtx.width;
    int ny = rtx.height;
    float aspect = float(nx) / float(ny);
    glm::vec3 lower_left_corner(-1.0f * aspect, -1.0f, -1.0f);
    glm::vec3 horizontal(2.0f * aspect, 0.0f, 0.0f);
    glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::mat4 world_from_view = glm::inverse(rtx.view);

    // You can try parallelising this loop by uncommenting this line:
    //#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < nx; ++x) {
        if  (rtx.show_aa){
            float u = ((float(x) + 0.5f) + (random_double())) / float(nx);
            float v = ((float(y) + 0.5f) + (random_double())) / float(ny);
            Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
        r.A = glm::vec3(world_from_view * glm::vec4(r.A, 1.0f));
        r.B = glm::vec3(world_from_view * glm::vec4(r.B, 0.0f));
        // Note: in the RTOW book, they have an inner loop for the number of
        // samples per pixel. Here, you do not need this loop, because we want
        // some interactivity and accumulate samples over multiple frames
        // instead (until the camera moves or the rendering is reset).

        if (rtx.current_frame <= 0) {
            // Here we make the first frame blend with the old image,
            // to smoothen the transition when resetting the accumulation
            glm::vec4 old = rtx.image[y * nx + x];
            rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
        }
        glm::vec3 c = color(rtx, r, rtx.max_bounces);
        //c = c / float(rtx.max_bounces);

        rtx.image[y * nx + x] += glm::vec4(c, 1.0f);
            }
        else { 
            float u = (float(x) + 0.5f) / float(nx);
            float v = (float(y) + 0.5f) / float(ny);
        
            Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            r.A = glm::vec3(world_from_view * glm::vec4(r.A, 1.0f));
            r.B = glm::vec3(world_from_view * glm::vec4(r.B, 0.0f));
            // Note: in the RTOW book, they have an inner loop for the number of
            // samples per pixel. Here, you do not need this loop, because we want
            // some interactivity and accumulate samples over multiple frames
            // instead (until the camera moves or the rendering is reset).

            if (rtx.current_frame <= 0) {
                // Here we make the first frame blend with the old image,
                // to smoothen the transition when resetting the accumulation
                glm::vec4 old = rtx.image[y * nx + x];
                rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
            }
            glm::vec3 c = color(rtx, r, rtx.max_bounces);
            //c = c / float(rtx.max_bounces);
            
            rtx.image[y * nx + x] += glm::vec4(c, 1.0f);}

        //if (rtx.show_aa){

            //float u = ((float(x) + 0.5f) + (random_double())) / float(nx);
            //float v = ((float(y) + 0.5f) + (random_double())) / float(ny);
        //}
        


        
    }
}

void updateImage(RTContext &rtx)
{
    if (rtx.freeze) return;                    // Skip update
    rtx.image.resize(rtx.width * rtx.height);  // Just in case...

    updateLine(rtx, rtx.current_line % rtx.height);

    updateScene(rtx);

    if (rtx.current_frame < rtx.max_frames) {
        rtx.current_line += 1;
        if (rtx.current_line >= rtx.height) {
            rtx.current_frame += 1;
            rtx.current_line = rtx.current_line % rtx.height;
        }
    }
}

void resetImage(RTContext &rtx)
{
    rtx.image.clear();
    rtx.image.resize(rtx.width * rtx.height);
    rtx.current_frame = 0;
    rtx.current_line = 0;
    rtx.freeze = false;
}

void resetAccumulation(RTContext &rtx)
{
    rtx.current_frame = -1;
}

}  // namespace rt
