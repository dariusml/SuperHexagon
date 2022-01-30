//
// Created by Darío on 18/01/2022.
//

#ifndef BASICS_ACCELEROMETER_EXAMPLE_OBSTACLEPATERN_H
#define BASICS_ACCELEROMETER_EXAMPLE_OBSTACLEPATERN_H

#include <vector>

//Guardo los posibles patrones de obstaculos que pueden aparecer en el juego
namespace hexagonGame{

    using namespace  std;

    /**
    *@brief Guarda parametros de un obstaculo
    */
    struct ObstacleData
    {

    private:
        int obstacleSlot;
        float obstacleSize;

    public:

        /**
        * @return: el slot donde se encuentra el obstaculo
        */
        const int & slot() const
        {
            return obstacleSlot;
        }
        /**
        * @return: la anchura del obstaculo
        */
        const float & size() const
        {
            return obstacleSize;
        }

    public:
        /**
         * @brief: constructor de la estructura de datos de obstaculo
        * @param slot: slot en el que se encuentra
         * @param size: anchura del obstaculo
        */
        ObstacleData(int slot, float size)
        {
            obstacleSlot = slot;
            obstacleSize = size;
        }

    };


    /**
    *@brief Guarda parametros del slice definido por su tiempo antes de spawnearse y sus obstaculos
    */
    class ObstacleSlice
    {

    private:
        vector<ObstacleData> obstaclesData;
        float timeOfSlice;
        int nObstacles = 0;

    public:
        /**
         * @return tiempo antes de spawnear
        */
        const float & getTimeToSlice() const
        {
            return timeOfSlice;
        }

        /**
         * @param obstacleIndex: index del obstaculo
        * @return slot en el que se encuentra obstaculo
        */
        const int & slot(int obstacleIndex) const
        {
            return obstaclesData[obstacleIndex].slot();
        }

        /**
        * @param obstacleIndex: index del obstaculo
        * @return anchura del obstaculo
        */
        const float & size(int obstacleIndex) const
        {
            return obstaclesData[obstacleIndex].size();
        }

        /**
        * @return numero de obstaculos de este slice
        */
        const int & getnObstacles()const
        {
            return  nObstacles;
        }

    public:

        /**
         * @brief: setea el tiempo que hay entre el anterior slice y este
        * @param timeOfSlice: tiempo antes de spawnearse el slice
        */
        void setTimeOfSlice(float timeOfSlice)
        {
            this->timeOfSlice = timeOfSlice;
        }

    public:
        ObstacleSlice() = default;

    public:

        /**
        * @brief: añade un obstaculo en este slice
        * @param obstacleData: datos del obstaculo
        */
        void addObstacle(const ObstacleData & obstacleData)
        {
            this->obstaclesData.push_back(obstacleData);
            ++nObstacles;
        }


    };

    /**
    *@brief Guarda parametros de posibles patrones de obstaculos definidos por slices
    */
    class ObstaclePattern
    {

    private:
        vector<ObstacleSlice> obstacleSlices;
        int nSlices = 0;

    public:

        /**
        * @param sliceIndex: index del slice
         * @return tiempo de spawneo del slice
        */
        const float & getSliceTime(int sliceIndex) const
        {
            return obstacleSlices[sliceIndex].getTimeToSlice();
        }

        /**
        * @param sliceIndex: index del slice
         * @param obstacleIndex: index del obstaculo de dentro del slice
        * @return slot del obstaculo pedido
        */
        const int & getobstacleSlot(int sliceIndex, int obstacleIndex) const
        {
            return obstacleSlices[sliceIndex].slot(obstacleIndex);
        }

        /**
        * @param sliceIndex: index del slice
        * @param obstacleIndex: index del obstaculo de dentro del slice
        * @return anchura del obstaculo pedido
        */
        const float & getobstacleSize(int sliceIndex, int obstacleIndex) const
        {
            return obstacleSlices[sliceIndex].size(obstacleIndex);
        }

        /**
        * @return numero de slices
        */
        const int & getnSlices() const
        {
            return nSlices;
        }

        /**
         * @param sliceIndex: index del slice
        * @return slice pedido
        */
        const ObstacleSlice & getSlice(int sliceIndex) const
        {
            return obstacleSlices[sliceIndex];
        }

    public:

        /**
         * @brief añade datos de obstaculo a patron de obstaculos
        * @param obstacleSlice: datos del slice a añadir
        */
        void addObstacleSlice(const ObstacleSlice & obstacleSlice)
        {
            this->obstacleSlices.push_back(obstacleSlice);
            ++nSlices;
        }


    };



}

#endif //BASICS_ACCELEROMETER_EXAMPLE_OBSTACLEPATERN_H
