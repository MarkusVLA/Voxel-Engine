#ifndef SKYBOX_H 
#define SKYBOX_H

#include <iostream>
#include <vector>
#include <memory>

class SkyBox{

public:
    SkyBox();
    ~SkyBox();

    /**
     * @brief Get poiner to the Vertices vector
     * 
     * @return std::vector<float>* 
     */
    std::vector<float> GetVertices();
    
private:
    std::vector<float> vertices_;
};


#endif // SKYBOX_H <