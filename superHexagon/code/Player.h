//
// Created by Darío on 15/01/2022.
//

#ifndef BASICS_ACCELEROMETER_EXAMPLE_PLAYER_H
#define BASICS_ACCELEROMETER_EXAMPLE_PLAYER_H


    #include "Polygon.hpp"

    namespace hexagonGame{

        using namespace  std;

        /**
         * @brief clase del jugador
        */
        class Player : public Polygon
        {
        private:
            float borderSize;
            float size;
            float speed;
            Point2f position;

            Point2f arrowVectorPositionLastFrame[3];

            float innerColor[3];

            float arroySeparation;


            Polygon arrow{3};
            Polygon * innerPolygon;

        public:

            /**
             * constructor del jugador
            * @param position: posicion del jugador
             * @param size: tamaño todal del jugador (borderSize no suma)
             * @param borderSize: grosor del borde del jugador
             * @param speed: velocidad de rotacion
            */
            Player(Point2f position,float size, float borderSize, float speed) : Polygon(6)
            {
                this->position = position;
                this->size = size;
                this->borderSize = borderSize;
                this->speed = speed;
                arroySeparation = size * 0.2;

                convertToHexagon(size,position);

                innerPolygon = new Polygon{nVertices};

                setArrow();
                setInnerPoligon();
            }

        public:


            void update(float deltaTime) override
            {

            }

            /**
            * renderiza el jugador y sus componentes(la flechita) y el borde
            */
            void render(Canvas &canvas) override
            {
                Polygon::render(canvas);
                innerPolygon->render(canvas);
                arrow.render(canvas);
                renderInnerPolygon(canvas);
            }

            /**
            * renderiza el jugador y sus componentes(la flechita) y el borde
             * @param zoom:zoom del jugador
             * @param pivot: lugar desde donde se hace el zoom
            */
            void render(Canvas &canvas, float zoom, Point2f pivot) override
            {
                Polygon::render(canvas,zoom, pivot);
                arrow.render(canvas,zoom, pivot);
                renderInnerPolygon(canvas,zoom, pivot);
            }

        private:
            void setArrow()
            {
                arrow.set_vertice_position(0,{position.coordinates.x(), size + position.coordinates.y() + arroySeparation });
                arrow.set_vertice_position(1,{position.coordinates.x() + arroySeparation, size + position.coordinates.y()});
                arrow.set_vertice_position(2,{position.coordinates.x() - arroySeparation, size + position.coordinates.y()});
            }

            void setInnerPoligon()
            {
                innerPolygon->convertToHexagon(size-borderSize,position);
            }


        public:

            /**
            * setea el color de dentro del jugador
             * @param r: valor de rojo
             * @param g: valor de verde
             * @param b: valor de azul
            */
            void  setInnerColor(const float r, const float g, const float b)
            {
                innerPolygon->set_color(r,g,b);
            }

            /**
            * setea el color de dentro del jugador
             * @param color: color a dar
            */
            void  setInnerColor(Color color)
            {
                innerPolygon->set_color(color);
            }

            /**
            * setea el color del jugador(bordes y flecha)
             * @param r: valor de rojo
             * @param g: valor de verde
             * @param b: valor de azul
            */
            void set_color(const float & r, const float & g, const float & b) override
            {
                Polygon::set_color(r,g,b);
                arrow.set_color(r,g,b);
            }

            /**
            * setea el color del jugador(bordes y flecha)
             * @param color: color a dar
            */
            void set_color(Color color) override
            {
                Polygon::set_color(color);
                arrow.set_color(color);
            }

            /**
            * setea la posicion de la flecha a la que tenía en el ultimo frame
            */
            void setArrowPositionToLastFrame()
            {
                arrow.set_vertice_position(0,{arrowVectorPositionLastFrame[0].coordinates.x(), arrowVectorPositionLastFrame[0].coordinates.y()});
                arrow.set_vertice_position(1,{arrowVectorPositionLastFrame[1].coordinates.x(), arrowVectorPositionLastFrame[1].coordinates.y()});
                arrow.set_vertice_position(2,{arrowVectorPositionLastFrame[2].coordinates.x(), arrowVectorPositionLastFrame[2].coordinates.y()});
            }

        public:

            /**
            * @return punta de la flecha del jugador
            */
            const Point2f & getTipOfArrow() const
            {
                return arrow.get_vertice_position(0);
            }

        private:
            void renderInnerPolygon(Canvas &canvas, float zoom, Point2f pivot)
            {
                canvas.set_color(innerColor[0], innerColor[1], innerColor[2]);
                innerPolygon->render(canvas,zoom,pivot);
            }
            void renderInnerPolygon(Canvas &canvas)
            {
                canvas.set_color(innerColor[0], innerColor[1], innerColor[2]);
                innerPolygon->render(canvas);
            }





        public:

            /**
            * rota alrededor de un punto
             * @param pivot: punto de pivote
             * @param angle: angulo en radianes que gira
            */
            void rotateAround(const Point2f &pivot, float angle) override
            {
                Polygon::rotateAround(pivot,angle);
                innerPolygon->rotateAround(pivot,angle);
                arrow.rotateAround(pivot,angle);
            }

            /**
            * rota alrededor de un punto
             * @param pivot: punto de pivote
             * @param angle: angulo en radianes que gira
            */
            void movePlayer(const Point2f & pivot, float angle)
            {
                //Consigo la ultima posicion del arrow en caso de que colisione de lado con algun poligono,
                //(la colision de lado no te mata, solo te para, como en el superHexagon) y en caso de SIDECOLLISION
                // vuelve a la posicion que tenia antes de colisionar
                arrowVectorPositionLastFrame[0] = { arrow.get_vertice_position(0).coordinates.x(), arrow.get_vertice_position(0).coordinates.y() };
                arrowVectorPositionLastFrame[1] = { arrow.get_vertice_position(1).coordinates.x(), arrow.get_vertice_position(1).coordinates.y() };
                arrowVectorPositionLastFrame[2] = { arrow.get_vertice_position(2).coordinates.x(), arrow.get_vertice_position(2).coordinates.y() };

                //rota dependiendo del angulo
                arrow.rotateAround(pivot,angle);
            }

        };

    }

#endif //BASICS_ACCELEROMETER_EXAMPLE_PLAYER_H
