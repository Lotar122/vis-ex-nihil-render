#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "FS.hpp"

namespace nihil {
	namespace nstd {
        struct Vertex {
            float x;
            float y;
            float z;

            friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
            {
                os << "X: " << vertex.x << ", Y: " << vertex.y << ", Z: " << vertex.z;
                return os;
            }
        };
        struct Face {
            float vi1;
            float ti1;
            float ni1;

            float vi2;
            float ti2;
            float ni2;

            float vi3;
            float ti3;
            float ni3;

            friend std::ostream& operator<<(std::ostream& os, const Face& face)
            {
                os << "vi1: " << face.vi1 << ", ti1: " << face.ti1 << ", ni1: " << face.ni1 <<
                    "vi2: " << face.vi2 << ", ti2: " << face.ti2 << ", ni2: " << face.ni2 <<
                    "vi3: " << face.vi3 << ", ti3: " << face.ti3 << ", ni3: " << face.ni3;
                return os;
            }
        };

        class OBJ 
        {
        public:
            std::string fileContent;
            std::vector<Vertex> vertices;
            std::vector<Face> faces;
            std::vector<float> verticesRender;
            std::vector<uint32_t> indicesRender;

            void Load(std::string& filepath, ScreenRatio screenRatio)
            {
                auto startL = std::chrono::high_resolution_clock::now();
                std::cout << "LOADING: " << std::endl;
                fileContent = LoadFile(filepath);
                auto endL = std::chrono::high_resolution_clock::now();
                std::cout << "LOADING took: " << std::chrono::duration_cast<std::chrono::milliseconds>(endL - startL).count() << std::endl;

                // Use std::istringstream to iterate over lines
                std::istringstream iss(fileContent);
                std::string line;

                size_t verticesSize = 0;
                size_t facesSize = 0;

                std::vector<std::string> verticesS;
                std::vector<std::string> facesS;

                std::cout << "PROCESSING: " << std::endl;
                auto startP = std::chrono::high_resolution_clock::now();

                while (std::getline(iss, line)) {
                    if (line.data()[0] == 'v' && line.data()[1] == ' ') {
                        verticesSize++;
                        verticesS.push_back(line);
                    }
                    else if (line.data()[0] == 'f' && line.data()[1] == ' ') {
                        facesSize++;
                        facesS.push_back(line);
                    }
                }

                vertices.resize(verticesSize);
                faces.resize(facesSize);

                verticesRender.resize(0);
                indicesRender.resize(0);

                std::cout << verticesSize << std::endl;

                for (int j = 0; j < vertices.size(); j++)
                {
                    std::vector<std::string> sv = splitString(verticesS[j], ' ');
                    Vertex v = {};
                    for (int i = 1; i < sv.size(); i++)
                    {
                        switch (i) {
                        case 1:
                            v.x = USC::NDC_u(std::stof(sv[i]) * 1000, screenRatio, WidthHeightEnum::Width);
                        break;
                        case 2:
                            v.y = USC::NDC_u(std::stof(sv[i]) * 1000, screenRatio, WidthHeightEnum::Height);
                        break;
                        case 3:
                            v.z = USC::NDC_u(std::stof(sv[i]) * 1000, screenRatio, WidthHeightEnum::Depth);
                        break;
                        }
                    }
                    vertices[j] = v;
                }

                for (int j = 0; j < facesSize; j++)
                {
                    std::vector<std::string> sv = splitString(facesS[j], ' ');
                    Face f = {};
                    std::vector<std::vector<std::string>> ssv(3);
                    for (int i = 1; i < sv.size(); i++)
                    {
                        switch (i) {
                        case 1:
                            ssv[i - 1] = splitString(sv[i], '/');
                            for (int z = 0; z < ssv[i - 1].size(); z++)
                            {
                                switch (z)
                                {
                                case 0:
                                    f.vi1 = std::stoi(ssv[i - 1][0]);
                                break;
                                case 1:
                                    f.ti1 = std::stoi(ssv[i - 1][1]);
                                    break;
                                case 2:
                                    f.ni1 = std::stoi(ssv[i - 1][2]);
                                    break;
                                }
                            }
                        break;
                        case 2:
                            ssv[i - 1] = splitString(sv[i], '/');
                            for (int z = 0; z < ssv[i - 1].size(); z++)
                            {
                                switch (z)
                                {
                                case 0:
                                    f.vi2 = std::stoi(ssv[i - 1][0]);
                                    break;
                                case 1:
                                    f.ti2 = std::stoi(ssv[i - 1][1]);
                                    break;
                                case 2:
                                    f.ni2 = std::stoi(ssv[i - 1][2]);
                                    break;
                                }
                            }
                            break;
                        case 3:
                            ssv[i - 1] = splitString(sv[i], '/');
                            for (int z = 0; z < ssv[i - 1].size(); z++)
                            {
                                switch (z)
                                {
                                case 0:
                                    f.vi3 = std::stoi(ssv[i - 1][0]);
                                    break;
                                case 1:
                                    f.ti3 = std::stoi(ssv[i - 1][1]);
                                    break;
                                case 2:
                                    f.ni3 = std::stoi(ssv[i - 1][2]);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    faces[j] = f;
                }

                for (int i = 0; i < vertices.size(); i++)
                {
                    verticesRender.push_back(vertices[i].x);
                    verticesRender.push_back(vertices[i].y);
                    verticesRender.push_back(vertices[i].z);
                    verticesRender.push_back(0.0f);
                    verticesRender.push_back(1.0f);
                    verticesRender.push_back(0.0f);
                }
                for (int i = 0; i < faces.size(); i++)
                {
                    indicesRender.push_back(faces[i].vi1 - 1);
                    indicesRender.push_back(faces[i].vi2 - 1);
                    indicesRender.push_back(faces[i].vi3 - 1);
                }

                auto endP = std::chrono::high_resolution_clock::now();

                std::cout << "PROCESSING took: "<< std::chrono::duration_cast<std::chrono::milliseconds>(endP - startP).count() <<std::endl;
            }
        };
	}
}