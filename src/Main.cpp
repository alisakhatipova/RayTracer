#include "Tracer.h"
#include "stdio.h"

using namespace glm;
void main(int argc, char** argv)
{
        CTracer tracer;
        CScene scene;
        #define G 6.674e-11
        #define c 3e+8
        int xRes = 1000, xRes_in;  // Default resolution
        int yRes = 750, yRes_in;
        double hole_mass = 8.57e+36, hole_mass_in;
        int disk_coef = 5, disk_coef_in;
        vec3 camera_pos(5* 12.7e+9, 0, 5 * 12.7e+9), camera_pos_in;
        vec3 camera_forward(-1, 0, -1), camera_forward_in;
        vec3 camera_up(-1, 0, 1), camera_up_in;
        vec3 camera_right(0, 1, 0), camera_right_in;
        vec3 earth1_pos_in, earth2_pos_in;
        double camera_view_angle = 50, camera_view_angle_in;
        if(argc == 2) // There is input file in parameters
        {
                FILE* file = fopen(argv[1], "r");
                if(file) {
                        int xResFromFile = 0;
                        int yResFromFile = 0;
                        if(fscanf(file, "%lf %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %lf %f %f %f  %f %f %f ", &hole_mass_in, &disk_coef_in, &xRes_in, &yRes_in,
                                    &camera_pos_in.x, &camera_pos_in.y, &camera_pos_in.z,
                                    &camera_forward_in.x, &camera_forward_in.y, &camera_forward_in.z,
                                    &camera_up_in.x, &camera_up_in.y, &camera_up_in.z,
                                    &camera_right_in.x, &camera_right_in.y, &camera_right_in.z,
                                    &camera_view_angle_in,
                                    &earth1_pos_in.x, &earth1_pos_in.y, &earth1_pos_in.z,
                                    &earth2_pos_in.x, &earth2_pos_in.y, &earth2_pos_in.z
                                     ) == 23) {
                               hole_mass = hole_mass_in;
                               disk_coef = disk_coef_in;
                               xRes = xRes_in;
                               yRes = yRes_in;
                               camera_pos = camera_pos_in;
                               camera_forward = camera_forward_in;
                               camera_up = camera_up_in;
                               camera_right = camera_right_in;
                               camera_view_angle = camera_view_angle_in;
                               scene.earth1_pos = earth1_pos_in;
                               scene.earth2_pos = earth2_pos_in;
                        } else {
                                printf("Invalid config format! Using default parameters.\r\n");
                        }

                        fclose(file);
                } else {
                        printf("Invalid config path! Using default parameters.\r\n");
                }
        } else {
                printf("No config! Using default parameters.\r\n");
        }
        tracer.m_camera.m_pixels.resize(xRes * yRes);
        scene.hole_mass = hole_mass;
        scene.disk_coeff = disk_coef;
        scene.hole_rad =  2 * G * hole_mass/ (c * c);
        tracer.m_camera.m_pos = camera_pos;
        tracer.m_camera.m_forward = normalize(camera_forward);
        tracer.m_camera.m_right = normalize(camera_right);
        tracer.m_camera.m_up = normalize(camera_up);
        tracer.m_camera.m_viewAngle = camera_view_angle;
        uvec2 res(xRes, yRes);
        tracer.m_camera.m_resolution = res;
        tracer.m_pScene = &scene;
        tracer.RenderImage(xRes, yRes);
        tracer.SaveImageToFile("Result.png");
        return ;
}
