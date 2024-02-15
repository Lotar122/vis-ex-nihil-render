#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>

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

            void Load(std::string filepath, ScreenRatio screenRatio)
            {
                fileContent = LoadFile(filepath);

                // Use std::istringstream to iterate over lines
                std::istringstream iss(fileContent);
                std::string line;

                size_t verticesSize = 0;
                size_t facesSize = 0;

                std::vector<std::string> verticesS;
                std::vector<std::string> facesS;

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

                std::vector<Vertex>* verticesP = &vertices;
                std::vector<float>* verticesRenderP = &verticesRender;
                std::vector<std::string>* verticesSP = &verticesS;

                std::thread verticesThread([verticesP, verticesRenderP, verticesSP, screenRatio]() {
                    std::vector<Vertex>* vertices = verticesP;
                    std::vector<float>* verticesRender = verticesRenderP;
                    std::vector<std::string>* verticesS = verticesSP;

                    for (int j = 0; j < vertices->size(); j++)
                    {
                        std::vector<std::string> sv = splitString((*verticesS)[j], ' ');
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
                        (*vertices)[j] = v;
                    }

                    uint8_t x = 1;

                    for (int i = 0; i < vertices->size(); i++)
                    {
                        x++;
                        verticesRender->push_back((*vertices)[i].x);
                        verticesRender->push_back((*vertices)[i].y);
                        verticesRender->push_back((*vertices)[i].z);
                        verticesRender->push_back(x * 0.1f);
                        verticesRender->push_back(x * 0.2f);
                        verticesRender->push_back(x * 0.3f);
                        if (x >= 3) x = 1;
                    }
                });

                std::vector<Face>* facesP = &faces;
                std::vector<uint32_t>* indicesRenderP = &indicesRender;
                std::vector<std::string>* facesSP = &facesS;

                std::thread indicesThread([facesP, indicesRenderP, facesSP]() {
                    std::vector<Face>* faces = facesP;
                    std::vector<uint32_t>* indicesRender = indicesRenderP;
                    std::vector<std::string>* facesS = facesSP;

                    for (int j = 0; j < faces->size(); j++)
                    {
                        std::vector<std::string> sv = splitString((*facesS)[j], ' ');
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
                        (*faces)[j] = f;
                    }

                    for (int i = 0; i < faces->size(); i++)
                    {
                        indicesRender->push_back((*faces)[i].vi1 - 1);
                        indicesRender->push_back((*faces)[i].vi2 - 1);
                        indicesRender->push_back((*faces)[i].vi3 - 1);
                    }
                });

                indicesThread.join();
                verticesThread.join();

                saveAsBinaryLib(filepath);
            }

            void saveAsBinaryLib(std::string path)
            {
                std::vector<std::vector<char>> files = makeBinaryLib();

                // Open a file for writing in binary mode
                std::ofstream outputFile(path + ".indices.bin", std::ios::binary);

                if (outputFile.is_open()) {
                    // Write the byte array to the file
                    outputFile.write(files[0].data(), files[0].size());

                    // Close the file
                    outputFile.close();

                    std::cout << "Binary File saved successfully." << std::endl;
                }
                else {
                    std::cerr << "Unable to open the file for writing." << std::endl;
                }

                // Open a file for writing in binary mode
                outputFile = std::ofstream(path + ".vertices.bin", std::ios::binary);

                if (outputFile.is_open()) {
                    // Write the byte array to the file
                    outputFile.write(files[1].data(), files[1].size());

                    // Close the file
                    outputFile.close();

                    std::cout << "Binary File saved successfully." << std::endl;
                }
                else {
                    std::cerr << "Unable to open the file for writing." << std::endl;
                }
            }

            void loadFromBinaryLib(std::string path)
            {
                // Specify the file name
                std::string fileName = path + ".indices.bin";

                // Open the file in binary mode
                std::ifstream fileStream(fileName, std::ios::binary);

                std::vector<char> indicesFile(0);

                if (fileStream.is_open()) {
                    // Determine the size of the file
                    fileStream.seekg(0, std::ios::end);
                    size_t fileSize = static_cast<size_t>(fileStream.tellg());
                    fileStream.seekg(0, std::ios::beg);

                    // Create a vector to hold the file content
                    indicesFile.resize(fileSize);

                    // Read the file into the vector
                    fileStream.read(indicesFile.data(), fileSize);

                    // Close the file
                    fileStream.close();

                    std::cout << "Binary File loaded successfully." << std::endl;
                }

                fileName = path + ".vertices.bin";
                fileStream = std::ifstream(fileName, std::ios::binary);
                
                std::vector<char> verticesFile(0);

                if (fileStream.is_open()) {
                    // Determine the size of the file
                    fileStream.seekg(0, std::ios::end);
                    size_t fileSize = static_cast<size_t>(fileStream.tellg());
                    fileStream.seekg(0, std::ios::beg);

                    // Create a vector to hold the file content
                    verticesFile.resize(fileSize);

                    // Read the file into the vector
                    fileStream.read(verticesFile.data(), fileSize);

                    // Close the file
                    fileStream.close();

                    std::cout << "Binary File loaded successfully." << std::endl;
                }

                // Ensure the size of the byte array is a multiple of sizeof(uint32_t)
                size_t numElements = indicesFile.size() / sizeof(uint32_t);

                indicesRender.resize(numElements);
                memcpy(indicesRender.data(), indicesFile.data(), indicesFile.size());

                numElements = verticesFile.size() / sizeof(float);

                verticesRender.resize(numElements);
                memcpy(verticesRender.data(), verticesFile.data(), verticesFile.size());
            }

            std::vector<std::vector<char>> makeBinaryLib() 
            {
                std::vector<char> indicesBinary;
                indicesBinary.resize(indicesRender.size() * sizeof(uint32_t));
                memcpy(indicesBinary.data(), indicesRender.data(), indicesBinary.size());

                std::vector<char> verticesBinary;
                verticesBinary.resize(verticesRender.size() * sizeof(float));
                memcpy(verticesBinary.data(), verticesRender.data(), verticesBinary.size());

                std::vector<std::vector<char>> finalFiles;
                finalFiles.push_back(indicesBinary);
                finalFiles.push_back(verticesBinary);

                return finalFiles;
            }
        };
	}
}