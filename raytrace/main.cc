#include "object.h"
#include "material.h"
#include "camera.h"
#include "ray.h"

using namespace CG;

int main() {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    std::shared_ptr<object> ground1 = std::make_shared<ground>(lambertianMaterial1, vec3(0, 0, 0), vec3(0, 1, 0));
    objects.push_back(ground1);

    std::shared_ptr<material> lambertianMaterial = std::make_shared<lambertian>(vec3(0.8, 0.5, 0.7));
    std::shared_ptr<object> sphere1 = std::make_shared<sphere>(lambertianMaterial, vec3(-20, 10, 15), 10);
    objects.push_back(sphere1);

    std::shared_ptr<material> metalMaterial = std::make_shared<metal>(vec3(0.9, 0.9, 0.9));
    std::shared_ptr<object> sphere2 = std::make_shared<sphere>(metalMaterial, vec3(20, 10, -15), 10);
    objects.push_back(sphere2);

    std::shared_ptr<material> dielectricMaterial = std::make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 5);
    std::shared_ptr<object> sphere3 = std::make_shared<sphere>(dielectricMaterial, vec3(0, 10, 0), 10);
    objects.push_back(sphere3);

    for (int x = -11; x <= 11; x++) {
        for (int z = -11; z <= 11; z++) {
            double typeRand = rand()%100;
            double xOffset = (double)(rand()%100)/(20.0) + 2.5;
            double zOffset = (double)(rand()%100)/(20.0) + 2.5;
            vec3 center(10.0*x + xOffset, 2, 10.0*z + zOffset);
            if ((center - vec3(20, 10, -10)).length() < 11
                || (center - vec3(-20, 10, 10)).length() < 11
                || (center - vec3(0, 10, 0)).length() < 11) {
                continue;
            }
            if (typeRand < 50) {
                std::shared_ptr<material> sphereSmallMaterial = std::make_shared<lambertian>(vec3::random(0.2, 1));
                std::shared_ptr<object> sphereSmall = std::make_shared<sphere>(sphereSmallMaterial, center, 2);
                objects.push_back(sphereSmall);
                continue;
            }
            if (typeRand < 60) {
                std::shared_ptr<material> sphereSmallMaterial = std::make_shared<metal>(vec3::random(0.2, 1), 0.2);
                std::shared_ptr<object> sphereSmall = std::make_shared<sphere>(sphereSmallMaterial, center, 2);
                objects.push_back(sphereSmall);
                continue;
            }
            if (typeRand < 85) {
                std::shared_ptr<material> sphereSmallMaterial = std::make_shared<metal>(vec3::random(0.7, 1));
                std::shared_ptr<object> sphereSmall = std::make_shared<sphere>(sphereSmallMaterial, center, 2);
                objects.push_back(sphereSmall);
                continue;
            }
            if (typeRand < 100) {
                std::shared_ptr<material> sphereSmallMaterial = std::make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 10);
                std::shared_ptr<object> sphereSmall = std::make_shared<sphere>(sphereSmallMaterial, center, 2);
                objects.push_back(sphereSmall);
                continue;
            }
        }
    }

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1.6, 2880, 1800, 1, Position(80, 10, 0, 0, -90, 0));

    uint32_t launchCount = 10;
    uint32_t samples = 100;
    cameraObject->render(objects, launchCount, samples);

    return 0;
}