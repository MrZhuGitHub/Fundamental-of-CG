#include "object.h"
#include "material.h"
#include "camera.h"
#include "ray.h"
#include "texture.h"

using namespace CG;

void RayTracingInOneWeekend() {
    std::vector<std::shared_ptr<object>> objects;

    // std::shared_ptr<material> lambertianMaterial = std::make_shared<lambertian>(vec3(0.8, 0.5, 0.7));
    // std::shared_ptr<object> sphere1 = std::make_shared<sphere>(lambertianMaterial, vec3(-20, 10, 15), 10);
    // objects.push_back(sphere1);

    // std::shared_ptr<material> lambertianMateria2 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    // std::shared_ptr<object> sphere2 = std::make_shared<sphere>(lambertianMateria2, vec3(20, 10, -15), 10);
    // objects.push_back(sphere2);

    // std::shared_ptr<material> lambertianMateria3 = std::make_shared<lambertian>(vec3(0.2, 0.4, 0.9));
    // std::shared_ptr<object> sphere3 = std::make_shared<sphere>(lambertianMateria3, vec3(0, 10, 0), 10);
    // objects.push_back(sphere3);

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    std::shared_ptr<object> ground1 = std::make_shared<sphere>(lambertianMaterial1, vec3(0, -100000, 0), 100000);
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
    uint32_t samples = 20;
    cameraObject->render(objects, launchCount, samples);
}

void rayTracingTheNextWeek() {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<material> light1 = std::make_shared<light>(color(10.0, 10.0, 10.0));
    std::shared_ptr<object> triangle1 = std::make_shared<triangle>(light1, vec3(30, 60, 30), vec3(30, 60, 70), vec3(70, 60, 30));
    std::shared_ptr<object> triangle2 = std::make_shared<triangle>(light1, vec3(70, 60, 70), vec3(30, 60, 70), vec3(70, 60, 30));
    objects.push_back(triangle1);
    objects.push_back(triangle2);

    std::shared_ptr<texture> texture1 = std::make_shared<texture>();
    if (!texture1->loadTexture("//opengles//Fundamental-of-CG//raytrace//earthmap.jpg")) {
        std::cout << "load Texture failed" << std::endl;
        return;
    }
    std::shared_ptr<material> lambertianMaterial2 = std::make_shared<lambertian>(vec3(0.9, 0.9, 0.9), texture1);
    std::shared_ptr<object> sphere3 = std::make_shared<sphere>(lambertianMaterial2, vec3(50, 25, 30), 10);
    sphere3->setGetTextureCoordinate(&sphere::getSphereTextureCoordinate);
    objects.push_back(sphere3);

    std::shared_ptr<material> metalMaterial = std::make_shared<metal>(vec3(0.8, 0.8, 0.9));
    std::shared_ptr<object> sphere1 = std::make_shared<sphere>(metalMaterial, vec3(30, 32, 20), 10);
    objects.push_back(sphere1);

    std::shared_ptr<material> dielectricMaterial = std::make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 1.5);
    std::shared_ptr<object> sphere2 = std::make_shared<sphere>(dielectricMaterial, vec3(70, 18, 10), 8);
    objects.push_back(sphere2);

    std::shared_ptr<material> metalMateria2 = std::make_shared<metal>(vec3(0.9, 0.9, 0.9), 0.2);
    std::shared_ptr<object> sphere4 = std::make_shared<sphere>(metalMateria2, vec3(10, 28, 15), 10);
    objects.push_back(sphere4);

    std::shared_ptr<material> metalMateria3 = std::make_shared<metal>(vec3(0.9, 0.9, 0.9), 0.8);
    std::shared_ptr<object> sphere5 = std::make_shared<sphere>(metalMateria3, vec3(20, 45, 80), 10);
    objects.push_back(sphere5);

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.48, 0.83, 0.53));
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            double x = 20 * i - 30;
            double y = rand()%10 - 10;
            double z = 20 * j - 30;
            std::shared_ptr<triangle> t1 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z), vec3(x, y+10, z), vec3(x, y+10, z+20));
            objects.push_back(t1);
            std::shared_ptr<triangle> t2 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z), vec3(x, y, z+20), vec3(x, y+10, z+20));
            objects.push_back(t2);
            std::shared_ptr<triangle> t3 = std::make_shared<triangle>(lambertianMaterial1, vec3(x+20, y, z), vec3(x+20, y+10, z), vec3(x+20, y+10, z+20));
            objects.push_back(t3);
            std::shared_ptr<triangle> t4 = std::make_shared<triangle>(lambertianMaterial1, vec3(x+20, y, z), vec3(x+20, y, z+20), vec3(x+20, y+10, z+20));
            objects.push_back(t4);
            std::shared_ptr<triangle> t5 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z), vec3(x, y+10, z), vec3(x+20, y+10, z));
            objects.push_back(t5);
            std::shared_ptr<triangle> t6 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z), vec3(x+20, y, z), vec3(x+20, y+10, z));
            objects.push_back(t6);
            std::shared_ptr<triangle> t7 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z+20), vec3(x, y+10, z+20), vec3(x+20, y+10, z+20));
            objects.push_back(t7);
            std::shared_ptr<triangle> t8 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z+20), vec3(x+20, y, z+20), vec3(x+20, y+10, z+20));
            objects.push_back(t8);
            std::shared_ptr<triangle> t9 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y+10, z), vec3(x+20, y+10, z), vec3(x, y+10, z+20));
            objects.push_back(t9);
            std::shared_ptr<triangle> t10 = std::make_shared<triangle>(lambertianMaterial1, vec3(x+20, y+10, z+20), vec3(x+20, y+10, z), vec3(x, y+10, z+20));
            objects.push_back(t10);
            std::shared_ptr<triangle> t11 = std::make_shared<triangle>(lambertianMaterial1, vec3(x, y, z), vec3(x+20, y, z), vec3(x, y, z+20));
            objects.push_back(t11);
            std::shared_ptr<triangle> t12 = std::make_shared<triangle>(lambertianMaterial1, vec3(x+20, y, z+20), vec3(x+20, y, z), vec3(x, y, z+20));
            objects.push_back(t12);
        }
    }

    std::shared_ptr<material> lambertianMaterial10 = std::make_shared<lambertian>(vec3(0.73, 0.73, 0.73), texture1);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                double x = i * 3 + 68 + ((double)(rand()%10000))/3000.0 - 1.7;
                double y = j * 3 + 22.7 + ((double)(rand()%10000))/3000.0 - 1.7;
                double z = k * 3 + 60 + ((double)(rand()%10000))/3000.0 - 1.7;
                std::shared_ptr<object> sphere0 = std::make_shared<sphere>(lambertianMaterial10, vec3(x, y, z), 2);
                objects.push_back(sphere0);
            }
        }
    }

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1.6, 1600, 1000, 1.5, Position(85, 30, -80, 0, -18, 0));
    uint32_t launchCount = 20;
    uint32_t samples = 10000;
    cameraObject->render(objects, launchCount, samples);  
}

void renderEarth() {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<texture> texture1 = std::make_shared<texture>();
    if (!texture1->loadTexture("//opengles//Fundamental-of-CG//raytrace//earthmap.jpg")) {
        std::cout << "load Texture failed" << std::endl;
        return;
    }

    std::shared_ptr<material> lambertianMaterial2 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    std::shared_ptr<object> ground1 = std::make_shared<sphere>(lambertianMaterial2, vec3(0, -100000, 0), 100000);
    objects.push_back(ground1);

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.9, 0.9, 0.9), texture1);
    std::shared_ptr<object> sphere3 = std::make_shared<sphere>(lambertianMaterial1, vec3(0, 10, 0), 10);
    sphere3->setGetTextureCoordinate(&sphere::getSphereTextureCoordinate);
    objects.push_back(sphere3);

    std::shared_ptr<material> light1 = std::make_shared<light>(color(15.0, 15.0, 15.0));
    std::shared_ptr<object> triangle1 = std::make_shared<triangle>(light1, vec3(-10, 27, -10), vec3(-10, 27, 10), vec3(10, 27, 10));
    std::shared_ptr<object> triangle2 = std::make_shared<triangle>(light1, vec3(-10, 27, -10), vec3(10, 27, -10), vec3(10, 27, 10));
    objects.push_back(triangle1);
    objects.push_back(triangle2);

    std::shared_ptr<material> light2 = std::make_shared<light>(color(10.0, 10.0, 10.0));
    std::shared_ptr<object> sphereLight = std::make_shared<sphere>(light2, vec3(15, 5, 15), 5);
    objects.push_back(sphereLight);

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1.6, 1600, 1000, 1.5, Position(80, 10, 0, 0, -90, 0));

    uint32_t launchCount = 10;
    uint32_t samples = 100;
    cameraObject->render(objects, launchCount, samples);   
}

void rayTracingTheRestOfYourLife() {

}

void cornellBox() {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.8, 0.5, 0.2));
    std::shared_ptr<material> lambertianMaterial2 = std::make_shared<lambertian>(vec3(0.2, 0.5, 0.8));
    std::shared_ptr<material> lambertianMaterial3 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    std::shared_ptr<material> lightMaterial1 = std::make_shared<light>(color(100.0, 100.0, 100.0));

    std::shared_ptr<object> triangle1 = std::make_shared<triangle>(lambertianMaterial1, vec3(0, 0, 0), vec3(0, 10, 0), vec3(0, 10, 10));
    std::shared_ptr<object> triangle2 = std::make_shared<triangle>(lambertianMaterial1, vec3(0, 0, 0), vec3(0, 0, 10), vec3(0, 10, 10));
    objects.push_back(triangle1);
    objects.push_back(triangle2);

    std::shared_ptr<object> triangle3 = std::make_shared<triangle>(lambertianMaterial2, vec3(10, 0, 0), vec3(10, 10, 0), vec3(10, 10, 10));
    std::shared_ptr<object> triangle4 = std::make_shared<triangle>(lambertianMaterial2, vec3(10, 0, 0), vec3(10, 0, 10), vec3(10, 10, 10));
    objects.push_back(triangle3);
    objects.push_back(triangle4);

    std::shared_ptr<object> triangle5 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 0, 0), vec3(0, 0, 10), vec3(10, 0, 10));
    std::shared_ptr<object> triangle6 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 0, 0), vec3(10, 0, 0), vec3(10, 0, 10));
    objects.push_back(triangle5);
    objects.push_back(triangle6);

    std::shared_ptr<object> triangle7 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 0, 10), vec3(10, 0, 10), vec3(10, 10, 10));
    std::shared_ptr<object> triangle8 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 0, 10), vec3(0, 10, 10), vec3(10, 10, 10));
    objects.push_back(triangle7);
    objects.push_back(triangle8);

    std::shared_ptr<object> triangle9 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 10, 0), vec3(0, 10, 10), vec3(10, 10, 10));
    std::shared_ptr<object> triangle10 = std::make_shared<triangle>(lambertianMaterial3, vec3(0, 10, 0), vec3(10, 10, 0), vec3(10, 10, 10));
    objects.push_back(triangle9);
    objects.push_back(triangle10);

    std::shared_ptr<object> light1 = std::make_shared<triangle>(lightMaterial1, vec3(4, 9.9, 4), vec3(4, 9.9, 6), vec3(6, 9.9, 4));
    std::shared_ptr<object> light2 = std::make_shared<triangle>(lightMaterial1, vec3(6, 9.9, 6), vec3(4, 9.9, 6), vec3(6, 9.9, 4));
    objects.push_back(light1);
    objects.push_back(light2);

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1, 1000, 1000, 1, Position(5, 5, -10, 0, 0, 0));
    uint32_t launchCount = 10;
    uint32_t samples = 200;
    cameraObject->render(objects, launchCount, samples);  
}

int main() {    
    rayTracingTheNextWeek();
    return 0;
}