//
// Created by Darío on 16/01/2022.
//

#ifndef OBSTACLEPOOL_H
#define OBSTACLEPOOL_H

    #include <random>
    #include <basics/Log>
    #include "Polygon.hpp"
    #include "ObstaclePatern.h"


    namespace hexagonGame{

    /**
    *@brief tipo de colision
    */
    enum TypeOfCollision
    {
        NOCOLLISION,
        SIDECOLLISION,
        FRONTCOLLISION
    };

        /**
        *@brief clase del obstaculo que puede colisionar con el jugador
        */
        class Obstacle : public Polygon
        {
        private:
            const Polygon * parentSection;
            bool active;

            float parentSize;
            float width;
            float speed;

            float proportion;
            float widthProportion;

            float lastFrameDistanceFromCenterSqr; //Necesario para el calculo de colisiones lateral

        public:

            /**
            * constructor de obstaculo que siempre tendra 4 vertices
            */
            Obstacle() : Polygon(4)
            {
                active = false;
                resetProportion();
            }


        public:

            /**
            *@return si esta activo
            */
            const bool getActive() const
            {
                return active;
            }

            /**
            *@return distancia recorrida en forma de proporcion
            */
            const float getProgress() const
            {
                return (1 - proportion);
            }

            /**
            *@return tamaño de la seccion donde se encuentra
            */
            const float & getParentSize() const
            {
                return parentSize;
            }

            /**
            *@return anchura proporcional a la seccion del obstaculo
            */
            const float & getWidthProportion() const
            {
                return widthProportion;
            }

        public:

            /**
             * set sobre el estado del obstaculo
            *@param active: estado que tendra el obstaculo
            */
            void setActive(bool active){
                this->active = active;
            }
            /**
             * set sobre la anchura del obstaculo
            *@param width: anchura del obstaculo
            */
            void setWidth(float width)
            {
                this->width = width;
            }

            /**
             * set sobre la seccion que contiene al obstaculo
            *@param parentSection: seccion a la que el obstaculo esta anclado
            */
            void setParentSection(const Polygon * parentSection)
            {
                this->parentSection = parentSection;

                //Consigo la longitud de esta sección
                float x = parentSection->get_vertice_position(1).coordinates.x() - parentSection->get_vertice_position(0).coordinates.x();
                float y = parentSection->get_vertice_position(1).coordinates.y() - parentSection->get_vertice_position(0).coordinates.y();
                parentSize = sqrtf( x*x + y*y);
            }

            /**
             * set sobre la velocidad del obstaculo
            *@param speed: velocidad del obstaculo
            */
            void setSpeed(const float & speed)
            {
                this->speed = speed;
            }

            /**
             * resetea la proporcion de avance del obstaculo
            */
            void resetProportion()
            {
                proportion = 1;
            }

        public:

            /**
            * coloca el obstaculo donde debe estar y ajusta su anchura
            */
            void update(float deltaTime)
            {
                lastFrameDistanceFromCenterSqr = SqrDistanceFromCenter(vertices[0]);

                float cosOfHalfAngle = cos(getAngleOfSection() * 0.5f);

                //seteo la posicion proporcional respecto a la seccion de los vertices más adelantados del obstaculo

                proportion -= (speed * deltaTime / cosOfHalfAngle) / parentSize * cosOfHalfAngle;

                //seteo la anchura proporcional respecto a la seccion de los vertices más adelantados del obstaculo
                widthProportion = width / cosOfHalfAngle / parentSize * cosOfHalfAngle;

                //consigo el centro donde se juntan todas las secciones
                Point2f centro = parentSection->get_vertice_position(0);

                //consigo el centro donde se juntan todas las seccionescoloco los vertices del obstaculo
                vertices[0] = makePointFromSegment(centro, parentSection->get_vertice_position(1), proportion);
                vertices[1] = makePointFromSegment(centro, parentSection->get_vertice_position(1), proportion + widthProportion);
                vertices[2] = makePointFromSegment(centro, parentSection->get_vertice_position(2), proportion + widthProportion);
                vertices[3] = makePointFromSegment(centro, parentSection->get_vertice_position(2), proportion);


            }

        private:
            //A partir de un segmento definido poe 2 puntos, escala el segmento en relacion al punto A
            Point2f makePointFromSegment(const Point2f A, const Point2f B, float proportion)
            {
                Point2f finalPoint;
                //Si ya ha llegado al centro, no continua
                if(proportion > 0)
                {
                    Vector2f vectorDiference{    B.coordinates.x() - A.coordinates.x(), B.coordinates.y() - A.coordinates.y()    };

                    vectorDiference *= proportion;

                    finalPoint = {   vectorDiference.coordinates.x() + A.coordinates.x(), vectorDiference.coordinates.y() + A.coordinates.y()   };
                } else{
                    finalPoint = {A.coordinates.x(), A.coordinates.y()};
                }

                return finalPoint;


            }

            float getAngleOfSection()
            {
                float dy;
                float dx;

                dy = parentSection->get_vertice_position(1).coordinates.y() - parentSection->get_vertice_position(0).coordinates.y();
                dx = parentSection->get_vertice_position(1).coordinates.x() - parentSection->get_vertice_position(0).coordinates.x();

                float angulo1 = atan2(dy,dx);

                dy = parentSection->get_vertice_position(2).coordinates.y() - parentSection->get_vertice_position(0).coordinates.y();
                dx = parentSection->get_vertice_position(2).coordinates.x() - parentSection->get_vertice_position(0).coordinates.x();

                float angulo2 = atan2(dy,dx);

                float anguloFinal = abs(angulo2 - angulo1) ;
                return anguloFinal;
            }

            float SqrDistanceFromCenter(const Point2f & point)
            {
                float dxObstacle = point.coordinates.x() - parentSection->get_vertice_position(0).coordinates.x();
                float dyObstacle = point.coordinates.y() - parentSection->get_vertice_position(0).coordinates.y();

                return (dxObstacle * dxObstacle + dyObstacle * dyObstacle);
            };

        public:
            /**
            * @param point: punto donde se comprueba la colision
             * @return tipo de colision
            */
            TypeOfCollision typeOfCollision(const Point2f & point)
            {
                TypeOfCollision typeOfCollision;

                float result;
                float currentSign = 0;
                bool inside = true;
                for(int i = 0; i < nVertices && inside; ++i)
                {
                    result = (point.coordinates.y() - vertices[i].coordinates.y()) * (vertices[(i+1)%nVertices].coordinates.x() - vertices[i].coordinates.x()) - (point.coordinates.x() - vertices[i].coordinates.x()) * (vertices[(i+1)%nVertices].coordinates.y() -vertices[i].coordinates.y());
                    if(currentSign == 0 && result != 0)
                    {
                        currentSign = result;
                    }

                    //mira si tiene el mismo signo que el anterior
                    if(   !(  (currentSign >= 0 && result >= 0) || (currentSign <= 0 && result <= 0)  )  ){
                        inside = false;
                    }

                }

                if(inside)
                {
                    //Distancia que tiene el punto con el centro
                    float dxPoint = point.coordinates.x() - parentSection->get_vertice_position(0).coordinates.x();
                    float dyPoint = point.coordinates.y() - parentSection->get_vertice_position(0).coordinates.y();

                    float tipDistanceFromCenterSqr = dxPoint * dxPoint + dyPoint * dyPoint;

                    if(lastFrameDistanceFromCenterSqr <= tipDistanceFromCenterSqr) typeOfCollision = SIDECOLLISION;
                    else typeOfCollision = FRONTCOLLISION;

                } else
                {
                    typeOfCollision = NOCOLLISION;
                }

                return typeOfCollision;


            }
        };



        /**
        *@brief pool de obstaculos que crea varios objetos obstaculos nada mas ser creada y luego los gestiona
        */
        class  ObstaclePool{

        private:
            int nObstacles;
            vector<Obstacle> obstacles;
            const Polygon * sections;
            float obstaclesSpeed;
            int currentNSections = 6;

            //A la obstaclePool se le enviaran ordenes de spawneo, variables necesarias
            bool spawning;
            ObstaclePattern patternToSpawn;
            int random; //Posicion inicial del pattern

            int nSlices;
            int currentSlice;
            float timeForNextSlice;


        public:

            /**
            * constructor de la clase
             * @param nObstacles: numero de obstaculos
             * @param sections: puntero a las secciones del obstaculo
            */
            ObstaclePool(int nObstacles, const Polygon * const sections)
            {
                this->nObstacles = nObstacles;
                this->sections = sections;
                spawning = false;
                for(int i = 0; i < nObstacles; ++i)
                {
                    obstacles.push_back( Obstacle{} );
                }
            }

        public:

            /**
            * spawnea patrones de obstaculos y lanza el update de aquellos activos
            */
            void update(float deltaTime)
            {

                if(spawning)
                {
                    updateSpawn(deltaTime);
                }

                for(int i = 0; i < nObstacles; ++i)
                {
                    if(obstacles[i].getActive())
                    {
                        obstacles[i].update(deltaTime);

                        //Si TODOS los vertices del obstaculo han llegado al final
                        if(obstacles[i].getProgress() >= 1 + obstacles[i].getWidthProportion())
                        {
                            deSpawn(i);
                        }
                    }
                }

            }

            /**
            * renderiza obstaculos activos
            */
            void render(Canvas & canvas)
            {
                for(int i = 0; i < nObstacles; ++i)
                {
                    if(obstacles[i].getActive())
                    {
                        obstacles[i].render(canvas);
                    }
                }
            }

            /**
             * @param zoom: zoom de los bstaculos
             * @param pivot: punto desde donde se spawnean los obstaculos
            * renderiza obstaculos activos
            */
            void render(Canvas & canvas, float zoom, Point2f pivot)
            {
                for(int i = 0; i < nObstacles; ++i)
                {
                    if(obstacles[i].getActive())
                    {
                        obstacles[i].render(canvas,zoom,pivot);
                    }
                }
            }


        public:

            /**
             * lanza un patron de obstaculos a spawnear
            * @param obstaclePattern: patron de obstaculos a spawnear
            */
            void spawnPattern(const ObstaclePattern & obstaclePattern)
            {
                this->patternToSpawn = obstaclePattern;
                spawning = true;
                nSlices = obstaclePattern.getnSlices();
                timeForNextSlice = obstaclePattern.getSliceTime(0);
                currentSlice = 0;

                //Genero un nRandom para decidir la posicion inicial del pattern


                random = rand() % 5;


            }

            /**
            * spawnea un obstaculo
            * @param parentSection: seccion al que pertence
             * @param width: anchura del obstaculo
            */
            const Obstacle & spawnObstacle(const Polygon * parentSection, float width)
            {
                Obstacle * obstacle;
                bool obstacleFound = false;
                for(int i = 0; i < nObstacles && !obstacleFound; ++i)
                {
                    if(!obstacles[i].getActive())
                    {
                        obstacle = &obstacles[i];
                        obstacleFound = true;
                    }
                }
                //Por si acaso la pool esta llena en un caso extremo que no deberia pasar nunca, creo un obstáculo mas
                if(!obstacleFound)
                {
                    obstacle = insertObstacleInVector();
                }

                obstacle->setWidth(width);
                obstacle->setActive(true);
                obstacle->setParentSection(parentSection);
                obstacle->resetProportion();


                return *obstacle;
            }

            /**
            * spawnea un obstaculo
            * @param indexOfSection: index de la seccion al que pertence
            * @param width: anchura del obstaculo
            */
            const Obstacle & spawnObstacle(int indexOfSection, float width)
            {
                return spawnObstacle(&sections[indexOfSection],width);
            }

            /**
            * desactiva un obstaculo, lo que lo hace despawnear
            */
            void deSpawn(int index)
            {
                obstacles[index].setActive(false);
            }

            /**
            * despawnea todos los obstaculos
            */
            void despawnAll()
            {
                spawning = false;
                for(int i = 0; i<nObstacles; ++i)
                {
                    deSpawn(i);
                }
            }

        private:

            //Ya que el spawneo de patrones se hace a lo largo del tiempo, necesitamos meterlo en el update
            void updateSpawn(float deltaTime)
            {
                if(timeForNextSlice <= 0)
                {
                    int nObstaclesOfSlice = patternToSpawn.getSlice(currentSlice).getnObstacles();
                    for(int i = 0; i < nObstaclesOfSlice; ++i)
                    {
                        spawnObstacle( ( patternToSpawn.getSlice(currentSlice).slot(i) + random) % currentNSections, patternToSpawn.getSlice(currentSlice).size(i) );
                    }
                    ++currentSlice;
                    if(currentSlice < nSlices) timeForNextSlice = patternToSpawn.getSliceTime(currentSlice);
                }
                if(currentSlice >= nSlices)
                {
                    spawning = false;
                }else{
                    timeForNextSlice -= deltaTime;
                }
            }


        private:
            Obstacle * insertObstacleInVector()
            {
                obstacles.push_back(Obstacle{});
                ++nObstacles;
                obstacles[nObstacles - 1].setSpeed(obstaclesSpeed);
                return &obstacles[nObstacles-1];
            }


        public:

            /**
            * @return obstaculo
            */
            const Obstacle & getObstacle(int index) const
            {

                if(index < 0 || index >= nObstacles) return obstacles[0];

                return obstacles[index];
            }

            /**
            * @return tipo ce colision
            */
            TypeOfCollision getCollision(const Point2f & point)
            {
                TypeOfCollision typeOfCollision = NOCOLLISION;
                for(int i = 0; i < nObstacles && typeOfCollision != FRONTCOLLISION; ++i)
                {
                    if(obstacles[i].getActive())
                    {
                        TypeOfCollision collision = obstacles[i].typeOfCollision(point);
                        //Si ya ha detectado una colision frontal sale del loop y devuelve FRONTCOLLISION, si no, en el caso de detectar
                        // una SIDECOLLISION temporalmente es una SIDECOLLISION hasta que acaba el loop o detecte una FRONTCOLLISION
                        if( collision == FRONTCOLLISION)
                        {
                            typeOfCollision = FRONTCOLLISION;
                        } else if( collision == SIDECOLLISION && typeOfCollision == NOCOLLISION ) typeOfCollision = SIDECOLLISION;
                    }
                }
                return typeOfCollision;
            }

            /**
            * @return si esta en proceso de spawnear un patron
            */
            const bool isSpawning() const
            {
                return spawning;
            }

        public:

            /**
             * Setea el color de los obstaculos
            * @param r: valor de rojo
             * @param g: valor de verde
             * @param b: valor de azul
            */
            void setObstaclesColor(const float & r,const float & g,const float & b)
            {
                for(int i = 0; i < nObstacles; ++i)
                {
                    obstacles[i].set_color(r,g,b);
                }
            }

            /**
             * Setea el color de los obstaculos
            * @param color: color al que setear
            */
            void setObstaclesColor(Color color)
            {
                for(int i = 0; i < nObstacles; ++i)
                {
                    obstacles[i].set_color(color);
                }
            }

            /**
             * Setea la velocidad de los obstaculos
            * @param speed: velocidad de los obstaculos
            */
            void setObstaclesSpeed(float speed)
            {
                obstaclesSpeed = speed;
                for(int i = 0; i < nObstacles; ++i)
                {
                    obstacles[i].setSpeed(obstaclesSpeed);
                }
            }



        };

}

#endif //BASICS_ACCELEROMETER_EXAMPLE_OBSTACLEPOOL_H
