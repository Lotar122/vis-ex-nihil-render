#include "Model.hpp"
#include "Classes/Engine/Engine.hpp"
#include "nihil-standard/nstd.hpp"

namespace nihil::graphics {
	Model::Model(Engine* _engine, glm::mat4 _deafultTransform)
	{
		engine = _engine;
		deafultTransform = _deafultTransform;

		obj.Load("./resources/models/cube.obj", nstd::LoadBinObj::DontCare, engine->app->screenRatio);

		vertices = obj.verticesRender;
		indices = obj.indicesRender;

		vBuffer = new Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>(engine, vertices);
		iBuffer = new Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>(engine, indices);
	}
	Model::Model(Engine* _engine, const std::string path, glm::mat4 _deafultTransform)
	{
		engine = _engine;
		deafultTransform = _deafultTransform;

		obj.Load(path, nstd::LoadBinObj::DontCare, engine->app->screenRatio);

		vertices = obj.verticesRender;
		indices = obj.indicesRender;

		vBuffer = new Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>(engine, vertices);
		iBuffer = new Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>(engine, indices);
	}
	Model::Model(Engine* _engine, nstd::OBJ _obj, glm::mat4 _deafultTransform)
	{
		engine = _engine;
		obj = _obj;
		deafultTransform = _deafultTransform;

		if (obj.verticesRender.size() <= 0 || obj.indicesRender.size() <= 0)
		{
			obj.Load("./resources/models/cube.obj", nstd::LoadBinObj::DontCare, engine->app->screenRatio);
		}

		vertices = obj.verticesRender;
		indices = obj.indicesRender;

		vBuffer = new Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>(engine, vertices);
		iBuffer = new Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>(engine, indices);
	}

	void Model::Load(const std::string path)
	{
		obj.Load(path, nstd::LoadBinObj::DontCare, engine->app->screenRatio);

		vertices = obj.verticesRender;
		indices = obj.indicesRender;

		delete vBuffer;
		delete iBuffer;

		vBuffer = new Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>(engine, vertices);
		iBuffer = new Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>(engine, indices);
	}
}