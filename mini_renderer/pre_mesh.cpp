#include "pre_mesh.h"
#include "ray.h"
#include "spherical_harmonics.h"

#include <random>
#include "bvh.h"
#include "spherical_harmonics.h"
#include "spherical_harmonics.inl"


static float Pi = 3.14159265358979323846;
static float InvPi = 0.31830988618379067154;
static float Inv2Pi = 0.15915494309189533577;
static float Inv4Pi = 0.07957747154594766788;
static float PiOver2 = 1.57079632679489661923;
static float PiOver4 = 0.78539816339744830961;

constexpr float EPS = 2e-4f;



class Frame
{
public:
	glm::vec3 x, y, z;

	Frame(glm::vec3 _x, glm::vec3 _y, glm::vec3 _z) :x(_x), y(_y), z(_z) {}

	static Frame from_z(glm::vec3 new_z)
	{
		new_z = glm::normalize(new_z);
		glm::vec3 new_y(0.f, 1.f, 0.f);
		if (std::abs(std::abs(glm::dot(new_z, glm::vec3(1.f, 0.f, 0.f))) - 1.f) < 0.1f)
			new_y = glm::cross(new_z, glm::vec3(0.f, 1.f, 0.f));
		else
			new_y = glm::cross(new_z, glm::vec3(1.f, 0.f, 0.f));
		return Frame(glm::cross(new_y, new_z), new_y, new_z);
	}

	glm::vec3 global_to_local(const glm::vec3& global_vector)const
	{
		return glm::vec3(glm::dot(x, global_vector), glm::dot(y, global_vector), glm::dot(z, global_vector));
	}

	glm::vec3 local_to_global(const glm::vec3& local_vector)const
	{
		return x * local_vector.x + y * local_vector.y + z * local_vector.z;
	}

};



class Sampler
{
	std::minstd_rand                      rng_;
	std::uniform_real_distribution<float> dis_;

public:

	explicit Sampler(int seed)
		: rng_(seed), dis_(0, 1)
	{

	}

	float sample()
	{
		return dis_(rng_);
	}

	glm::vec2 sample2()
	{
		const float s1 = sample();
		const float s2 = sample();
		return glm::vec2(s1, s2);
	}
};


/**
 * @brief 测试一个浮点数是否具有有限值
 */
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool is_finite(T val) noexcept
{
	return !std::isnan(val) && !std::isinf(val);
}


void computeVertexSHNoShadow(
	float         coef,
	const Ray& ray,
	int           SHCount,
	float* output)
{
	if (!is_finite(coef))
		return;

	auto SHFuncs = spherical_harmonics::linear_table();
	for (int i = 0; i < SHCount; ++i)
		output[i] += coef * SHFuncs[i](ray.d.x,ray.d.y,ray.d.z);
}

void computeVertexSHShadow(
	float      coef,
	const Ray& ray,
	const BVH& bvh,
	int        SHCount,
	float* output)
{
	if (!is_finite(coef))
		return;

	if (bvh.hasIntersection(ray))
		return;

	auto SHFuncs = spherical_harmonics::linear_table();
	for (int i = 0; i < SHCount; ++i)
		output[i] += coef * SHFuncs[i](ray.d.x,ray.d.y,ray.d.z);
}


std::pair<glm::vec3,float> cosineSampleHemisphere(float u1, float u2) noexcept
{
	glm::vec2 sam;
	u1 = 2 * u1 - 1;
	u2 = 2 * u2 - 1;
	if (u1 || u2)
	{
		float theta, r;
		if (std::abs(u1) > std::abs(u2))
		{
			r = u1;
			theta = PiOver4* (u2 / u1);
		}
		else
		{
			r = u2;
			theta = PiOver2 - PiOver4 * (u1 / u2);
		}

		sam = r * glm::vec2(std::cos(theta),std::sin(theta));
	}

	const float  z = std::sqrt(std::max(0.f, 1.f - sam.x * sam.x - sam.y * sam.y));
	return { glm::vec3(sam.x,sam.y,z), z * InvPi};
}


void computeVertexSHInterRefl(
	const SHVertex* vertices,
	float           coef,
	float           brdf,
	Ray             ray,
	const BVH& bvh,
	int             SHCount,
	int             maxDepth,
	Sampler& sampler,
	float* output)
{
	auto SHFuncs = spherical_harmonics::linear_table();

	for (int depth = 1; depth <= maxDepth; ++depth)
	{
		if (!is_finite(coef))
			return;

		BVH::Intersection inct;
		if (!bvh.findIntersection(ray, &inct))
		{
			for (int i = 0; i < SHCount; ++i)
				output[i] += coef * SHFuncs[i](ray.d.x, ray.d.y, ray.d.z);
			return;
		}

		const SHVertex* tri = &vertices[3 * inct.triangle];
		const glm::vec3 nor =
			glm::normalize((1 - inct.uv.x - inct.uv.y) * tri[0].normal +
				inct.uv.x * tri[1].normal +
				inct.uv.y * tri[2].normal);


		if (dot(nor, ray.d) >= 0)
			return;

		const glm::vec2 sam = sampler.sample2();
		auto [local_dir, pdf_dir] = cosineSampleHemisphere(sam.x, sam.y);

		glm::vec3 tets();

		const Frame localFrame = Frame::from_z(nor);
		ray.d = glm::normalize(localFrame.local_to_global(local_dir));
		ray.o = inct.position + EPS * nor;

		//coef *= brdf * std::abs(cos(ray.d, nor)) / pdf_dir;
		float cos_dir_nor = glm::dot(ray.d, nor) / (ray.d.length()*nor.length());
		coef *= brdf * std::abs(cos_dir_nor) / pdf_dir;
	}
}

std::vector<float> computeVertexSHCoefs(
	const SHVertex* vertices,
	int             vertexCount,
	float           vertexAlbedo,
	int             maxOrder,
	int             samplesPerVertex,
	LightingMode    mode)
{
	assert(vertices && vertexCount > 0);
	assert(vertexCount % 3 == 0);
	assert(samplesPerVertex > 0);

	const float brdf = vertexAlbedo / Pi;
	const float invSamplesPerVertex = 1.0f / samplesPerVertex;

	BVH bvh;
	if (mode != LightingMode::NoShadow)
	{
		std::vector<glm::vec3> positions(vertexCount);
		for (int vi = 0; vi < vertexCount; ++vi)
			positions[vi] = vertices[vi].position;
		bvh = BVH::create(positions.data(), vertexCount / 3);
	}

	const int SHCount = (maxOrder+1)*(maxOrder + 1);
	std::vector<float> result(SHCount * vertexCount, 0.0f);

	const int reportStepSize = (std::max)(vertexCount / 50, 1);
	int lastReportedVi = 0;

	#pragma omp parallel for
	for(int vi = 0 ; vi < vertexCount; vi++)
	{
			Sampler sampler(vi);

			auto& vertex = vertices[vi];
			float* output = &result[SHCount * vi];

			const glm::vec3 o = vertex.position + EPS * vertex.normal;
			const Frame localFrame = Frame::from_z(vertex.normal);

			#pragma omp parallel for
			for (int si = 0; si < samplesPerVertex; ++si)
			{
				const auto sam = sampler.sample2();
				const auto [localDir, pdfDir] = cosineSampleHemisphere(sam.x, sam.y);

				const glm::vec3 d = glm::normalize(localFrame.local_to_global(localDir));
				const Ray ray(o, d);

				float cos_d_nor = glm::dot(d, vertex.normal) / (d.length() * vertex.normal.length());

				const float initCoef = brdf * std::abs(cos_d_nor) / pdfDir;

				if (mode == LightingMode::NoShadow)
					computeVertexSHNoShadow(initCoef, ray, SHCount, output);
				else if (mode == LightingMode::Shadow)
					computeVertexSHShadow(initCoef, ray, bvh, SHCount, output);
				else
				{
					computeVertexSHInterRefl(
						vertices, initCoef, brdf, ray, bvh,
						SHCount, 5, sampler, output);
				}
			}

			for (int i = 0; i < SHCount; ++i)
				output[i] *= invSamplesPerVertex;


			printf("progress %f \n", float(vi) / vertexCount);
	}

	return result;
}


