/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Raster_Font>
#include <rapidxml.hpp>
#include "Polygon.hpp"
#include "Player.h"
#include "ObstaclePool.h"
#include "ObstaclePatern.h"
#include "Color.h"

namespace hexagonGame
{

    using basics::Texture_2D;
    using basics::Graphics_Context;

    /**
    * @brief escena de juego
    */
    class Sample_Scene : public basics::Scene
    {

        /**
        * Direccion del jugador
        */
        enum DirectionState
        {
            LEFT = -1,
            NEUTRAL = 0,
            RIGHT = 1
        };

        enum State
        {
            LOADING,
            READY,
            FINISHED,
            ERROR
        };

        struct BackButton
        {
            float width;
            float height;
            Point2f position;
            float   is_pressed;
        };


    private:

        bool aspect_ratio_adjusted;
        bool canvasDependantAspectRatio;

        State state;
        BackButton backButton;
        float backButtonWidth;
        Point2f backButtonPosition;

        bool     suspended;                         ///< true cuando la aplicación está en segundo plano

        unsigned canvas_width;                      ///< Resolución virtual del display
        unsigned canvas_height;
        Point2f sceneCenter;


        list<Polygon*> polygonList;                  ///< lista de todos las secciones de la escena


        static constexpr int nSections = 6;
        float sectionSize;
        Polygon sections[nSections];

        float playerBorderSize;
        float playerSize;
        float playerSpeed;
        Player * player;
        DirectionState playerDirection;

        ObstaclePool * obstaclePool;
        float obstacleSpeed;
        vector<ObstaclePattern> obstaclePatterns;

        //Rotacion de mapa

        float secondsBetweenDirectionChange;
        float secondsSinceDirectionChange;
        int rotationDirection;
        float rotationSpeed;



        //parámetros para el renderizado y el color

        vector<ColorSet> colorSets;
        ColorSet currentColorSet{};

        int currentColorSetIndex;

        float timeBetwwenColorChange;
        float timeOfColor;
        float timeSinceColor;
        float timeBetweenColorChangeSections;
        float timeSinceColorChangeSections;
        bool firstSectionClear; //Si es false, la primera seccion es color oscuro, si no, claro

        //variables necesarias para el beat visual en el que el personaje se hace mas grande y mas pequeño
        float playerMaxZoom;
        float playerMinZoom;
        float playerCurrentZoom;
        float beatVelocity;
        float obstaclesZoom;
        bool beat = false; //Si está en proceso de agrandarse


        //Parametros para la muerte y la animación de muerte
        bool dead;
        bool animationDeathFinished;
        float deathZoomTime; //Cuando muere se hace un zoom al jugador
        float currentDeathZoomTime;
        float maxDeathZoom;
        float currentDeathZoom;

        //Parametros para reset del level
        bool resetingLevel;
        float resetZoomTime; //Cuando muere se hace un zoom al jugador
        float timeSinceResetingLevel;


        std::shared_ptr < Texture_2D > backTexture;
        bool backTextureAdded;



    public:

        /**
        * default 1280 * 720
        */
        Sample_Scene();

        /**
        * Este metodo lo llama Director para conocer la resolucion virtual con la que esta¡
        * trabajando la escena.
        * @return TamaÃ±o en coordenadas virtuales que esta usando la escena.
        */
        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        /**
         * Aqui se inicializan los atributos que deben restablecerse cada vez que se inicia la escena.
         */
        bool initialize () override;
        /**
        * Este metodo lo invoca Director automaticamente cuando el juego pasa a segundo plano.
        */
        void suspend () override;
        /**
        * Este metodo lo invoca Director automaticamente cuando el juego pasa a primer plano.
        */
        void resume () override;

        /**
        * Este metodo se invoca automaticamente una vez por fotograma para que la escena
        * actualize su estado.
        */
        void update (float ) override;

        /**
        * Este metodo se invoca automaticamente una vez por fotograma para que la escena
        * dibuje su contenido.
        */
        void render (basics::Graphics_Context::Accessor & context) override;


    private:

        //Renderiza las secciones del mapa
        void renderMap(Canvas & canvas);

        //inserta poligono en la lista de poligonos del mapa
        void insertPolygonInList(Polygon* & polygon);

        /**Esta funcion se encarga de comprobar si tiene que cambiar la
        * direccion de rotacion del mapa
        */
        void checkForRotationChange(float deltaTime);

        /**Esta funcion rota el mapa
        * direccion de rotacion del mapa
        */
        void rotateMap(float angleSeconds, float deltaTime);

        //crea las secciones del mapa
        void createSections(float horizontalLength);

        //movimiento del personaje
        void movimiento(float deltaTime);

        //spawnea patrones de obstaculos aleatorios
        void spawnObstacles();


        /**Esta funcion se encarga de Lerpear ColorSets y actualizar
         * el timer de cambio de color de las secciones
        */
        void colorSetRotation(float deltaTime);

        //setea el color de la escena a un oolorSet
        void setSceneColor(ColorSet & colorSet);

        //se encarga de decidir el zoom que habra en el personaje para el efecto de zoom de este
        void updateZoomBeat(float deltaTime);

        //Pantalla de muerte/pausa (en el superHexagon es lo mismo)
        void pauseDeathScreen()
        {
            obstaclePool->setObstaclesSpeed(0);
            dead = true;
        }

        //animacion de muerte que hace zoom hasta el personaje
        void animationDeath(float deltaTime);

        //animacion de revivir que hace zoom hacia atras hasta ver el mapa otra vez
        void animationReset(float deltaTime);

        //resetea el nivel despues de pausa o muerte
        void resetLevel();

        //Pone el boton de back/pausa y setea los parametros
        void setBackButton(const Point2f & proportionalPosition)
        {
            float proportion = backTexture->get_width() / backTexture->get_height();
            backButton.width = backButtonWidth;
            backButton.height = backButtonWidth / proportion;
            backButton.position = {proportionalPosition[0] * canvas_width, proportionalPosition[1] * canvas_height};
        }

        //Pone el boton de back/pausa y setea los parametros
        bool touchingBackButton(Point2f touchPosition)
        {
            bool touching = false;
            for (int index = 0; index < 4; ++index)
            {
                if
                        (
                        touchPosition[0] > backButton.position[0] - backButton.width/2  &&
                        touchPosition[0] < backButton.position[0] + backButton.width/2  &&
                        touchPosition[1] > backButton.position[1] - backButton.height/2 &&
                        touchPosition[1] < backButton.position[1] +backButton.height/2
                        )
                {
                    touching = true;
                }
            }

            return touching;
        }

        //renderiza el backBoton de back/pausa y setea los parametros
        void showBackButton(Canvas & canvas)
        {
            if (backTexture)
            {
                canvas.fill_rectangle
                        (
                                { backButton.position[0], backButton.position[1] },
                                { backButton.width, backButton.height},
                                backTexture. get ()
                        );
            }
        }



        //recibe inputs
        void handle (basics::Event & event) override;


        /** Este metodo lee el archivo XML e interpreta su contenido.
 */
        void load_xml ();

        /** Este metodo parsea el tag <scene> que es la raiz del XML.
         */
        void parse_scene_node (rapidxml::xml_node< > * );

        /** Este metodo parsea los parametros de la escena <scene> que es la raiz del XML.
        */
        void parse_sceneParam_node (rapidxml::xml_node< > * );

        /** Este metodo parsea los patrones de spawneo de obstaculos de la escena <scene> que es la raiz del XML.
        */
        ObstaclePattern parse_ObstaclePattern_node(rapidxml::xml_node< > * );

    private:

        //Metodos necesarios para el parseo de los mencionados

        ObstacleSlice parse_ObstacleSlice_node(rapidxml::xml_node< > * );

        ObstacleData parse_Obstacle_node(rapidxml::xml_node< > * );




        ColorSet parse_colorSet_node(rapidxml::xml_node< > * );

        Color parse_color_node(rapidxml::xml_node< > * );



        float parse_float_node(rapidxml::xml_node< > * );



        Point2f parse_position_node(rapidxml::xml_node< > * );

        void parse_text_node (rapidxml::xml_node< > * );


    private:

        void adjustAspectRatio(Graphics_Context::Accessor & context)
        {
            if (!aspect_ratio_adjusted)
            {
                float real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();
                canvas_width = unsigned( canvas_height * real_aspect_ratio );
                aspect_ratio_adjusted = true;
            }
        }

        void loadSceneEsentials();

    };

}
