#include "object.h"
#include "material.h"
#include "camera.h"
#include "ray.h"
#include "texture.h"

using namespace CG;

void RayTracingInOneWeekend(uint32_t samples = 20) {
    std::vector<std::shared_ptr<object>> objects;

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
    printf("render RayTracingInOneWeekend: samples = %d, launchCount = %d\n", samples, launchCount);
    cameraObject->render(objects, launchCount, samples, "./RayTracingInOneWeekend.ppm", true);
}

void rayTracingTheNextWeek(uint32_t samples = 1000) {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<material> light1 = std::make_shared<light>(color(1, 1, 1));
    std::shared_ptr<object> parallelogram1 = std::make_shared<parallelogram>(light1, vec3(40, 70, 25), vec3(40, 70, 40), vec3(55, 70, 40));
    objects.push_back(parallelogram1);

    std::shared_ptr<texture> texture1 = std::make_shared<texture>();
    if (!texture1->loadTexture("//opengles//Fundamental-of-CG//raytrace//earthmap.jpg")) {
        std::cout << "load Texture failed" << std::endl;
        return;
    }
    std::shared_ptr<material> lambertianMaterial2 = std::make_shared<lambertian>(vec3(0.9, 0.9, 0.9), texture1);
    std::shared_ptr<object> sphere3 = std::make_shared<sphere>(lambertianMaterial2, vec3(50, 21, 25), 10);
    sphere3->setGetTextureCoordinate(&sphere::getSphereTextureCoordinate);
    objects.push_back(sphere3);

    std::shared_ptr<material> metalMaterial = std::make_shared<metal>(vec3(0.8, 0.8, 0.9));
    std::shared_ptr<object> sphere1 = std::make_shared<sphere>(metalMaterial, vec3(30, 32, 20), 10);
    objects.push_back(sphere1);

    std::shared_ptr<material> dielectricMaterial = std::make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 3);
    std::shared_ptr<object> sphere2 = std::make_shared<sphere>(dielectricMaterial, vec3(75, 18, 10), 8);
    objects.push_back(sphere2);

    std::shared_ptr<material> metalMateria2 = std::make_shared<metal>(vec3(0.9, 0.9, 0.9), 0.2);
    std::shared_ptr<object> sphere4 = std::make_shared<sphere>(metalMateria2, vec3(10, 28, 10), 10);
    objects.push_back(sphere4);

    std::shared_ptr<material> metalMateria3 = std::make_shared<metal>(vec3(0.9, 0.9, 0.9), 0.8);
    std::shared_ptr<object> sphere5 = std::make_shared<sphere>(metalMateria3, vec3(30, 50, 60), 10);
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

    lambertian* lm1 = dynamic_cast<lambertian*>(lambertianMaterial1.get());
    lm1->addLightObject(parallelogram1);
    lambertian* lm2 = dynamic_cast<lambertian*>(lambertianMaterial2.get());
    lm2->addLightObject(parallelogram1);
    lambertian* lm10 = dynamic_cast<lambertian*>(lambertianMaterial10.get());
    lm10->addLightObject(parallelogram1);

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1.6, 1600, 1000, 1.5, Position(85, 30, -80, 0, -18, 0));
    uint32_t launchCount = 10;
    printf("render rayTracingTheNextWeek: samples = %d, launchCount = %d\n", samples, launchCount);
    cameraObject->render(objects, launchCount, samples, "./rayTracingTheNextWeek.ppm");  
}

void renderEarth(uint32_t samples = 100) {
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
    printf("render earth: samples = %d, launchCount = %d\n", samples, launchCount);
    cameraObject->render(objects, launchCount, samples, "./earth.ppm");   
}

void rayTracingTheRestOfYourLife(uint32_t samples = 1000) {
    std::vector<std::shared_ptr<object>> objects;

    std::shared_ptr<material> lambertianMaterial1 = std::make_shared<lambertian>(vec3(0.8, 0.5, 0.2));
    std::shared_ptr<material> lambertianMaterial2 = std::make_shared<lambertian>(vec3(0.2, 0.5, 0.8));
    std::shared_ptr<material> lambertianMaterial3 = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    std::shared_ptr<material> lightMaterial1 = std::make_shared<light>(color(6.0, 6.0, 6.0));

    std::shared_ptr<object> parallelogram1 = std::make_shared<parallelogram>(lambertianMaterial1, vec3(0, 10, 0), vec3(0, 0, 0), vec3(0, 0, 10));
    objects.push_back(parallelogram1);

    std::shared_ptr<object> parallelogram2 = std::make_shared<parallelogram>(lambertianMaterial2, vec3(10, 0, 0), vec3(10, 10, 0), vec3(10, 10, 10));
    objects.push_back(parallelogram2);

    std::shared_ptr<object> parallelogram3 = std::make_shared<parallelogram>(lambertianMaterial3, vec3(0, 0, 0), vec3(0, 0, 10), vec3(10, 0, 10));
    objects.push_back(parallelogram3);

    std::shared_ptr<object> parallelogram4 = std::make_shared<parallelogram>(lambertianMaterial3, vec3(0, 0, 10), vec3(10, 0, 10), vec3(10, 10, 10));
    objects.push_back(parallelogram4);

    std::shared_ptr<object> parallelogram5 = std::make_shared<parallelogram>(lambertianMaterial3, vec3(0, 10, 0), vec3(0, 10, 10), vec3(10, 10, 10));
    objects.push_back(parallelogram5);

    std::shared_ptr<object> light1 = std::make_shared<parallelogram>(lightMaterial1, vec3(4, 9.9, 4), vec3(4, 9.9, 6), vec3(6, 9.9, 6));
    objects.push_back(light1);

    std::shared_ptr<material> metalMaterial = std::make_shared<metal>(vec3(1.0, 1.0, 1.0));
    std::shared_ptr<object> sphere1 = std::make_shared<sphere>(metalMaterial, vec3(7.5, 2, 6), 2);
    objects.push_back(sphere1);

    std::shared_ptr<material> dielectricMaterial = std::make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 3);
    std::shared_ptr<object> sphere2 = std::make_shared<sphere>(dielectricMaterial, vec3(2.5, 2, 4), 2);
    objects.push_back(sphere2);

    lambertian* lm1 = dynamic_cast<lambertian*>(lambertianMaterial1.get());
    lm1->addLightObject(light1);
    lambertian* lm2 = dynamic_cast<lambertian*>(lambertianMaterial2.get());
    lm2->addLightObject(light1);
    lambertian* lm3 = dynamic_cast<lambertian*>(lambertianMaterial3.get());
    lm3->addLightObject(light1);

    std::shared_ptr<camera> cameraObject = std::make_shared<camera>(1, 1, 1000, 1000, 1, Position(5, 5, -10, 0, 0, 0));
    uint32_t launchCount = 10;
    printf("render rayTracingTheRestOfYourLife: samples = %d, launchCount = %d\n", samples, launchCount);
    cameraObject->render(objects, launchCount, samples, "./rayTracingTheRestOfYourLife.ppm");  
}

int main(int argc, char** argv) {
    if ((5 != argc) || (0 != strcmp(argv[1], "-example")) || (0 != strcmp(argv[3], "-samples"))) {
        printf("input error params");
        return -1;
    }
    int type = std::stoi(argv[2]);
    uint32_t samples = std::stoi(argv[4]);
    switch (type)
    {
    case 1:
        RayTracingInOneWeekend(samples);
        break;
    case 2:
        rayTracingTheNextWeek(samples);
        break;
    case 3:
        rayTracingTheRestOfYourLife(samples);
        break;
    case 0:
        renderEarth(samples);
        break;    
    default:
        printf("type = %d is invalid\n", type);
        return -1;
        break;
    }
    
    return 0;
}