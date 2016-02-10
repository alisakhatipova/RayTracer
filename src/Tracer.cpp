#include "Tracer.h"

using namespace glm;

SRay CTracer::MakeRay(glm::uvec2 pixelPos, float coef1, float coef2)
{
        float PI = 3.141592;
        SRay ray;
        ray.m_start = m_camera.m_pos;
        float angle = m_camera.m_viewAngle * (PI / 180.0f);
        float angle_tan = tan(angle);
        float h_half = m_camera.m_resolution.y / 2, w_half = m_camera.m_resolution.x / 2;
        float h = m_camera.m_resolution.y, w =  m_camera.m_resolution.x;
        float dist = h_half / angle_tan;
        vec3 ViewDir = dist * m_camera.m_forward;
        vec3 Up = m_camera.m_up * h;
        vec3 Right = m_camera.m_right * w;
        float carry1 = (pixelPos.x + coef1) * 1.0f/w - 0.5, carry2 = (pixelPos.y + coef2) * 1.0f/h - 0.5;
        ray.m_dir = ViewDir + carry1 * Right + carry2 * Up;
        float c = 3e+8;
        vec3 norm = normalize(ray.m_dir);
        ray.m_dir = norm * c;
        return ray;
}


float CTracer::RayFlatnessIntersection(SRay ray){
    float t = -1;
    vec3 norm = normalize(ray.m_dir);
    ray.m_dir = norm;
    if (abs(ray.m_dir.z) > 1e-9)
        t = -ray.m_start.z / ray.m_dir.z;
    return t;
}


float CTracer::RaySphereIntersection(SRay ray, vec3 spos, float r)
{
  float t = -1;
  vec3 norm = normalize(ray.m_dir);
  ray.m_dir = norm;
  vec3 k = ray.m_start - spos;
  float b = dot(k,ray.m_dir);
  float c = dot(k,k) - r*r;
  float d = b*b - c;

  if(d >=0)
  {
    float sqrtfd = sqrtf(d);
    // t, a == 1
    float t1 = -b + sqrtfd;
    float t2 = -b - sqrtfd;

    float min_t  = min(t1,t2);
    float max_t = max(t1,t2);

    t = (min_t >= 0) ? min_t : max_t;
  }
  return t;
}


glm::vec3 CTracer::TraceRay(SRay ray)
{
        float PI = 3.141592;
        float d_t = 1;
        vec3 color_disk(0, 0, 0);
        vec3 color(0, 0, 0);
        float alpha = 0;
        float c = 3e+8;
        while (1) {
            vec3 r = -ray.m_start;
            float r_len = length(r);
            float G = 6.674e-11;
            float carry1 = G * m_pScene->hole_mass;
            carry1 /= r_len; carry1 /= r_len; carry1 /= r_len;
            vec3 a = r * carry1;
            vec3 oldstart = ray.m_start;
            vec3 olddir = ray.m_dir;
            ray.m_dir += a * d_t;
            vec3 carry = normalize(ray.m_dir) * c;
            ray.m_dir = carry;
            ray.m_start += olddir * d_t + a *( d_t * d_t / 2);
            float cos_beetw = dot(normalize(olddir), normalize(ray.m_dir));
            SRay ray1;
            ray1.m_dir = normalize(ray.m_start - oldstart);
            ray1.m_start = oldstart;
            float t1 = RaySphereIntersection(ray1, vec3(0, 0, 0), m_pScene->hole_rad);

            float t = RayFlatnessIntersection(ray1);
            if ((t > 0) &&(length(ray1.m_dir * t) < length(ray.m_start - oldstart)) && (length(ray1.m_start + ray1.m_dir * t) < m_pScene->hole_rad * m_pScene->disk_coeff)&& (length(ray1.m_start + ray1.m_dir * t) >= m_pScene->hole_rad )) {
                int w = disk->GetWidth(), h = disk->GetHeight();
                vec3 pos = ray1.m_start + ray1.m_dir * t;
                float xx = pos.x / (m_pScene->hole_rad * m_pScene->disk_coeff);
                xx *= w/2;
                xx += w/2;
                float yy = pos.y / (m_pScene->hole_rad * m_pScene->disk_coeff);
                yy *= h/2;
                yy += h/2;
                unsigned char* pix = static_cast<unsigned char*>(disk->GetPixelAddress(xx,yy));
                if (pix[3] == 0)
                    continue;
                color_disk = vec3(pix[2], pix[1], pix[0])/255.0f;
                alpha = pix[3] * 1.0f / 255;
                //break;
            }
            if ((t1 > 0) &&(length(ray1.m_dir * t1) < length(ray.m_start - oldstart)))
                break;

            t = RaySphereIntersection(ray1, m_pScene->earth1_pos, m_pScene->earth_rad);
            if ((t > 0) && (length(ray1.m_dir * t) < length(ray.m_start - oldstart))){
                float phi = (atan2(ray1.m_dir.x, ray1.m_dir.y) + PI)/ (2 * PI);
                float teta = (asin(ray1.m_dir.z) + PI/2) / PI;
                int w = earth->GetWidth(), h = earth->GetHeight();
                int ii = w * phi, jj = h * teta;
                if (ii == w) ii = w - 1;
                if (jj == h) jj = h - 1;
                unsigned char* pix = static_cast<unsigned char*>(earth->GetPixelAddress(ii, jj));
                color = vec3(pix[2], pix[1], pix[0])/255.0f;
                break;
            }
            t = RaySphereIntersection(ray1, m_pScene->earth2_pos, m_pScene->earth_rad);
            if ((t > 0) && (length(ray1.m_dir * t) < length(ray.m_start - oldstart))){
                float phi = (atan2(ray1.m_dir.x, ray1.m_dir.y) + PI)/ (2 * PI);
                float teta = (asin(ray1.m_dir.z) + PI/2) / PI;
                int w = earth->GetWidth(), h = earth->GetHeight();
                int ii = w * phi, jj = h * teta;
                if (ii == w) ii = w - 1;
                if (jj == h) jj = h - 1;
                unsigned char* pix = static_cast<unsigned char*>(earth->GetPixelAddress(ii,jj));
                color = vec3(pix[2], pix[1], pix[0])/255.0f;
                break;
            }

            if ((abs(cos_beetw) > 0.999999) && (length(oldstart) < length(ray.m_start)) ){
                ray.m_dir = normalize(ray.m_dir);
                float phi = (atan2(ray.m_dir.x, ray.m_dir.y)+PI) / (2 * PI);
                float teta = (asin(ray.m_dir.z) + PI/2) / PI;
                int w = stars->GetWidth(), h = stars->GetHeight();
                int ii = w * phi, jj = h * teta;
				if (ii >= w) ii = 0;
				if (jj >= h) jj = 0;
                unsigned char* pix = static_cast<unsigned char*>(stars->GetPixelAddress(ii, jj));
                color = vec3(pix[2], pix[1], pix[0])/255.0f;
                break;
            }
        }
        return alpha * color_disk + (1 - alpha) * color;
}


void CTracer::RenderImage(int xRes, int yRes)
{
  // Reading input texture sample
  disk = LoadImageFromFile("data/disk_32.png");
  stars = LoadImageFromFile("data/stars.png");
  earth = LoadImageFromFile("data/fire.png");
 /* for(int i = 0; i < pImage->GetHeight(); i+= pitch) // Image lines
    {
      for(int j = 0; j < pImage->GetWidth(); j++) // Pixels in line

        unsigned char b = pCurrentLine[j * 4 + i];
        unsigned char g = pCurrentLine[j * 4 + 1 + i];
        unsigned char r = pCurrentLine[j * 4 + 2 + i];
        unsigned char alpha = pCurrentLine[j * 4 + 3 + i];
      }
    }
  }
 */
  // Rendering
  m_camera.m_resolution = uvec2(xRes, yRes);
  m_camera.m_pixels.resize(xRes * yRes);

  for(int i = 0; i < yRes; i++)
    for(int j = 0; j < xRes; j++)
    {
                        vec3 finishcolor(0, 0, 0), carrycolor(0, 0, 0);
                        SRay ray = MakeRay(uvec2(j, i), 0.25, 0.5);
                        carrycolor = TraceRay(ray);
                        finishcolor += carrycolor;
                        ray = MakeRay(uvec2(j, i), 0.75, 0.5);
                        carrycolor = TraceRay(ray);
                        finishcolor += carrycolor;
                        ray = MakeRay(uvec2(j, i), 0.5, 0.25);
                        carrycolor = TraceRay(ray);
                        finishcolor += carrycolor;
                        ray = MakeRay(uvec2(j, i), 0.5, 0.75);
                        carrycolor = TraceRay(ray);
                        finishcolor += carrycolor;
                        finishcolor /= 4.0f;
                        m_camera.m_pixels[i * xRes + j] = finishcolor;
    }
}

void CTracer::SaveImageToFile(std::string fileName)
{
  CImage image;

  int width = m_camera.m_resolution.x;
  int height = m_camera.m_resolution.y;

  image.Create(width, height, 24);

	int pitch = image.GetPitch();
	unsigned char* imageBuffer = (unsigned char*)image.GetBits();

	if (pitch < 0)
	{
		imageBuffer += pitch * (height - 1);
		pitch =- pitch;
	}

	int i, j;
	int imageDisplacement = 0;
	int textureDisplacement = 0;

	for (i = 0; i < height; i++)
	{
    for (j = 0; j < width; j++)
    {
      vec3 color = m_camera.m_pixels[textureDisplacement + j];

      imageBuffer[imageDisplacement + j * 3] = clamp(color.b, 0.0f, 1.0f) * 255.0f;
      imageBuffer[imageDisplacement + j * 3 + 1] = clamp(color.g, 0.0f, 1.0f) * 255.0f;
      imageBuffer[imageDisplacement + j * 3 + 2] = clamp(color.r, 0.0f, 1.0f) * 255.0f;
    }

		imageDisplacement += pitch;
		textureDisplacement += width;
	}

  image.Save(fileName.c_str());
	image.Destroy();
}

CImage* CTracer::LoadImageFromFile(std::string fileName)
{
  CImage* pImage = new CImage;

  if(SUCCEEDED(pImage->Load(fileName.c_str())))
    return pImage;
  else
  {
    delete pImage;
    return NULL;
  }
}
