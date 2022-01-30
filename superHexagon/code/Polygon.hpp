/*
 * SPRITE
 * Copyright © 2021+ Darío Muñoz Lanuza
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 */

#ifndef POLYGON_HEADER
#define POLYGON_HEADER

    #include <memory>
    #include <basics/Canvas>
    #include <basics/Vector>
    #include "Color.h"


    namespace hexagonGame
    {

        using basics::Canvas;
        using basics::Size2f;
        using basics::Point2f;
        using basics::Vector2f;
        using namespace  std;

        /**
         * @brief clase padre poligono de la que lo visual en la escena hereda
        */
        class Polygon
        {

        protected:

            uint nVertices;
            vector<Point2f> vertices;               ///< Vertices que definen el poligono
            float color[3];

            bool         visible = true;                   ///< Indica si el sprite se debe actualizar y dibujar o no. Por defecto es true.

        public:
            /**
             * contructor
             * @param numero de vertices del poligono
            */
            Polygon(const size_t nVertices)
            {
                set_color(1,1,1);

                this->nVertices = nVertices;
                this->vertices.resize(nVertices);
            }

            /**
             * contructor por defecto con 3 vertices
            */
            Polygon() : Polygon(3){}


            ~Polygon() = default;


        public:

            // Getters:
            /**
             * consigue posicion de vertice seleccionado
             * @param vertice: index del vertice
             * @return posicion de vertice seleccionado
            */
            const Point2f  & get_vertice_position(const  size_t & vertice) const  {return vertices[vertice];}

            /**
             * @return si es visible o no
            */
            bool is_not_visible () const
            {
                return !visible;
            }

        public:

            // Setters:
            /**
             * setea posicion de vertice seleccionado
             * @param nVertice: index del vertice a cambiar
             * @param new_position: nueva posicion del vertice
             * @return posicion de vertice seleccionadoç
            */
            void set_vertice_position (const  size_t & nVertice, const Point2f & new_position)
            {
                vertices[nVertice] = new_position;
            }

            /**
            * setea el color del poligono
             * @param r: valor de rojo
             * @param g: valor de verde
             * @param b: valor de azul
            */
            virtual void  set_color(const float & r, const float & g, const float & b)
            {
                color[0] = r;
                color[1] = g;
                color[2] = b;
            }

            /**
            * setea el color del poligono
             * @param color: color
            */
            virtual void  set_color(Color color)
            {
                this->color[0] = color.red();
                this->color[1] = color.green();
                this->color[2] = color.blue();
            }



        public:

            /**
             * Hace que el sprite no se actualice ni se dibuje.
             */
            void hide ()
            {
                visible = false;
            }

            /**
             * Hace que el sprite no se actualice ni se dibuje.
             */
            void show ()
            {
                visible = true;
            }

        public:

            /**
             * Se ejecuta cada frame
             * solo cuando es visible.
             * @param time Fracción de tiempo que se debe avanzar.
             */
            virtual void update (float deltaTime){}

            /**
             * Dibuja el poligono a base de dibujar triangulos, pero solo cuando es visible.
             * @param canvas Referencia al Canvas que se debe usar para dibujar la imagen.
             */
            virtual void render (Canvas & canvas)
            {
                if (visible)
                {
                    canvas.set_color (color[0], color[1], color[2]);
                    for(int i = 1; i+1 < nVertices; ++i)
                    {
                        canvas.fill_triangle(vertices[0],vertices[i],vertices[i +1] );
                    }
                }

            }

            /**
            * Dibuja el poligono a base de dibujar triangulos y fakea zoom , pero solo cuando es visible.
             * @param canvas Referencia al Canvas que se debe usar para dibujar la imagen.
             * @param zoom es el zoom.
             * @param pivot el punto de refernecia donde se fakea el zoom
            */
            virtual void render (Canvas & canvas, float zoom, Point2f pivot)
            {
                if (visible)
                {
                    canvas.set_color (color[0], color[1], color[2]);

                    Point2f pointAroundPivot = { vertices[0].coordinates.x() - pivot.coordinates.x(), vertices[0].coordinates.y() - pivot.coordinates.y() };

                    Point2f fakeVertice0 = {pointAroundPivot.coordinates.x() * zoom + pivot.coordinates.x(), pointAroundPivot.coordinates.y() * zoom + pivot.coordinates.y()};

                    for(int i = 1; i+1 < nVertices; ++i)
                    {
                        pointAroundPivot = { vertices[i].coordinates.x() - pivot.coordinates.x(), vertices[i].coordinates.y() - pivot.coordinates.y() };
                        Point2f fakeVertice1 = {pointAroundPivot.coordinates.x() * zoom + pivot.coordinates.x(), pointAroundPivot.coordinates.y() * zoom + pivot.coordinates.y()};

                        pointAroundPivot = { vertices[i + 1].coordinates.x() - pivot.coordinates.x(), vertices[i + 1].coordinates.y() - pivot.coordinates.y() };
                        Point2f fakeVertice2 = {pointAroundPivot.coordinates.x() * zoom + pivot.coordinates.x(), pointAroundPivot.coordinates.y() * zoom + pivot.coordinates.y()};

                        canvas.fill_triangle(fakeVertice0,fakeVertice1,fakeVertice2 );
                    }
                }

            }


        public:

            /**
            * rota el poligono dependiendo de un pivote y un angulo
             * @param pivot: punto de pivote
             * @param angle: angulo que rota en radianes
            */
           virtual void rotateAround(const Point2f & pivot, float angle)
            {
                for(int i = 0; i < nVertices; ++i)
                {
                    rotatePoint(pivot, angle, vertices[i]);
                }
            }

        protected:
            void rotatePoint(const Point2f & pivot,float & angle, Point2f & point)
            {
                //Hago que el puntod pivote cuente como punto(0,0) para facilitar los calculos
                Point2f pointAroundOrigin = { point.coordinates.x() - pivot.coordinates.x(), point.coordinates.y() - pivot.coordinates.y() };

                float newX = pointAroundOrigin.coordinates.x() * cos(-angle) - pointAroundOrigin.coordinates.y() * sin(-angle);
                float newY = pointAroundOrigin.coordinates.x() * sin(-angle) + pointAroundOrigin.coordinates.y() * cos(-angle);

                point = { newX + pivot.coordinates.x(), newY + pivot.coordinates.y() };
            }

        public:

            /**
            * crea un hexagono reguloar
             * @param length: distancia del centro a sus vertices
             * @param position: posicion del centro del hexagono
             * @return Polygono que es un hexagono regular
            */
            //PREFERIRIA PONER ESTE METODO EN .CPP, PERO SI LO HAGO NO FUNCIONA en movil, aqui en el compilador de PC sí
            static  Polygon createRegularHexagon(float length, const Point2f position)
            {
                Polygon hexagon{6};

                //variables utiles
                float xOffset = position.coordinates.x();
                float yOffset = position.coordinates.y();
                //Saco los vertices de un hexagono, para poder hacer las secciones
                float xValue = length*0.5f;
                float yValue = sqrtf(3) * length * 0.5f;


                hexagon.set_vertice_position(0,  {xValue  + xOffset, yValue  + yOffset}   );
                hexagon.set_vertice_position(1,  {length  + xOffset,           yOffset}   );
                hexagon.set_vertice_position(2,  {xValue  + xOffset, -yValue + yOffset}   );
                hexagon.set_vertice_position(3,  {-xValue + xOffset, -yValue + yOffset}   );
                hexagon.set_vertice_position(4,  {-length + xOffset,           yOffset}   );
                hexagon.set_vertice_position(5,  {-xValue + xOffset, yValue  + yOffset}   );

                return  hexagon;
            }

        public:

            /**
            * convierte poligono a hexagono
             * @param length: distancia del centro a sus vertices
             * @param position: posicion del centro del hexagono
             * @return Polygono que es un hexagono regular
            */
            void convertToHexagon(float length, const Point2f position)
            {
                vertices.clear();

                float xOffset = position.coordinates.x();
                float yOffset = position.coordinates.y();
                //Saco los vertices de un hexagono, para poder hacer las secciones
                float xValue = length*0.5f;
                float yValue = sqrtf(3) * length * 0.5f;

                vertices.push_back({xValue  + xOffset, yValue  + yOffset});
                vertices.push_back({length  + xOffset,           yOffset});
                vertices.push_back({xValue  + xOffset, -yValue + yOffset});
                vertices.push_back({-xValue + xOffset, -yValue + yOffset} );
                vertices.push_back({-length + xOffset,           yOffset} );
                vertices.push_back({-xValue + xOffset, yValue  + yOffset});
            }


        };



    }

#endif
