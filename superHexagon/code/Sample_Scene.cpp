/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Sample_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <sstream>
#include <basics/Asset>

using namespace basics;
using namespace rapidxml;
using namespace std;

namespace hexagonGame
{
    Sample_Scene::Sample_Scene()
    {
        //Constructor sampleScene aaaaaaaaaaaa
        state         = LOADING;
        aspect_ratio_adjusted = false;
        canvasDependantAspectRatio = false;
        canvas_width  = 1280;                   // Todavía no se puede calcular el aspect ratio, por lo que se establece
        canvas_height =  720;                   // un tamaño por defecto hasta poder calcular el tamaño final


        //direccion inicial de rotacion del jugador
        playerDirection = NEUTRAL;




        //Rotacion de mapa parametros iniciales necesarios(timers etc)
        secondsSinceDirectionChange = 0;
        rotationDirection = 1;


        //parámetros para el renderizado iniciales necesarios(timers etc)
        currentColorSetIndex = 0;
        timeSinceColor =0;
        timeSinceColorChangeSections = 0;
        firstSectionClear = false;

        //variables necesarias iniciales para el beat visual en el que el personaje se hace mas grande y mas pequeño
        playerCurrentZoom = 1;
        beat = false; //Si está en proceso de agrandarse


        //Parametros para la muerte y la animación de muerte
        dead = false;
        animationDeathFinished = false;
        currentDeathZoomTime = 0;
        currentDeathZoom = 0;

        backTextureAdded = false;
    }

    void Sample_Scene::suspend ()
    {
        suspended = true;
    }

    void Sample_Scene::resume ()
    {
        suspended = false;
    }

    bool  Sample_Scene::initialize()
    {
        suspended = false;





        return true;
    }

    void Sample_Scene::update (float deltaTime)
    {

        if (!suspended) if (state == LOADING)
            {
            loadSceneEsentials();
            }
        if(state == READY)
        {
            basics::log.d("UPDATE1" + to_string(playerCurrentZoom) + "    " + to_string(currentDeathZoom) + "    ");
            
            checkForRotationChange(deltaTime);
            rotateMap(rotationSpeed,deltaTime);

            //Si estaba muerto y está reiniciando hace zoom hacia atras
            if(resetingLevel)
            {
                animationReset(deltaTime);
            }

            if(!dead)
            {
                movimiento(deltaTime);

                spawnObstacles();

                colorSetRotation(deltaTime);

                setSceneColor(currentColorSet);

            } else{
                animationDeath(deltaTime);
            }
            updateZoomBeat(deltaTime);
            obstaclePool->update(deltaTime);

            //obsevamos colision la cual puede ser de 2 tipos, colision de frente(Te mata) y colision de lado, que no te deja avanzar a esa dirección más más
            TypeOfCollision collision = obstaclePool->getCollision( player->getTipOfArrow());
            if(collision == FRONTCOLLISION )
            {
                pauseDeathScreen();
            } else if(collision == SIDECOLLISION)
            {
                player->setArrowPositionToLastFrame();
            }
        }

    }

    void Sample_Scene::render (basics::Graphics_Context::Accessor & context)
    {
        if (!suspended && aspect_ratio_adjusted)
        {
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if(!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }
            if (canvas)
            {
                canvas->clear ();

                renderMap(*canvas);
                obstaclePool->render(*canvas, obstaclesZoom + currentDeathZoom, sceneCenter);
                player->render(*canvas, playerCurrentZoom + currentDeathZoom, sceneCenter);
                showBackButton(*canvas);
            }
        }

    }



    void Sample_Scene::renderMap(Canvas & canvas)
    {
        list<Polygon*>::iterator it;
        for (it = polygonList.begin(); it != polygonList.end(); ++it){
            (*it)->render(canvas);
        }
    }
    void Sample_Scene::insertPolygonInList(Polygon* & polygon)
    {
        polygonList.push_front(polygon);
    }

    void Sample_Scene::checkForRotationChange(float deltaTime)
    {
        secondsSinceDirectionChange += deltaTime;
        if(secondsSinceDirectionChange >= secondsBetweenDirectionChange)
        {
            secondsSinceDirectionChange = 0;
            rotationDirection *= -1;
        }
    }
    void Sample_Scene::rotateMap(float angleSeconds, float deltaTime)
    {
        float angle = angleSeconds * rotationDirection;
        list<Polygon*>::iterator it;
        for (it = polygonList.begin(); it != polygonList.end(); ++it){
            (*it)->rotateAround(sceneCenter,angle * deltaTime);
        }
        player->rotateAround(sceneCenter,angle * deltaTime);
    }

    void Sample_Scene::createSections(float length)
    {
        Polygon hexagon = Polygon::createRegularHexagon(length,sceneCenter);



        //creo las secciones
        for(unsigned  i = 0; i < nSections; ++i)
        {
            sections[i] = Polygon(3);

            sections[i].set_vertice_position(0,sceneCenter);
            sections[i].set_vertice_position(1,  hexagon.get_vertice_position(i)                                     );
            sections[i].set_vertice_position(2,  hexagon.get_vertice_position((i+1)%nSections)   );

            polygonList.push_front(&sections[i]); //Meto las secciones en la lista de poligonos
        }

    }

    void Sample_Scene::movimiento(float deltaTime)
    {
        float velocidad = playerSpeed * playerDirection * deltaTime;
        player->movePlayer(sceneCenter, velocidad);
    }

    void Sample_Scene::spawnObstacles()
    {
        if(!obstaclePool->isSpawning())
        {
            int random = rand() % obstaclePatterns.size();

            obstaclePool->spawnPattern( obstaclePatterns[random] );
        }
    }



    void Sample_Scene::colorSetRotation(float deltaTime)
    {
        timeSinceColor += deltaTime;
        timeSinceColorChangeSections +=deltaTime;
        if(timeSinceColor >= timeOfColor)
        {
            //La proporcion la necesito para la interpolacion lineal de colores;
            float proportion = (timeSinceColor - timeOfColor) / timeBetwwenColorChange;
            if(proportion >= 1)
            {
                currentColorSetIndex = (currentColorSetIndex + 1) % colorSets.size();
                timeSinceColor = 0;
                currentColorSet = colorSets[currentColorSetIndex];

            }
            else
            {

                int nextColorSetIndex = (currentColorSetIndex + 1) % colorSets.size();

                currentColorSet = ColorSet::lerp(colorSets[currentColorSetIndex], colorSets[nextColorSetIndex], proportion);

            }

        }


        //Actualiza si a una seccion le toca color oscuro, o claro
        if(timeSinceColorChangeSections > timeBetweenColorChangeSections)
        {
            timeSinceColorChangeSections = 0;
            firstSectionClear = !firstSectionClear;
        }
    }

    void Sample_Scene::setSceneColor(ColorSet & colorSet)
    {
        obstaclePool->setObstaclesColor(colorSet.getMainColor());
        player->set_color(colorSet.getMainColor());
        player->setInnerColor(colorSet.getDarkSectionColor());

        for(size_t i = 0; i<nSections;++i)
        {
            if( (i + int(firstSectionClear)) %2 == 0)
            {
                sections[i].set_color(colorSet.getDarkSectionColor());
            } else{
                sections[i].set_color(colorSet.getClearSectionColor());
            }
        }
    }


    void Sample_Scene::updateZoomBeat(float deltaTime)
    {
        if(playerCurrentZoom < playerMinZoom)
        {
            beat = true;
        }
        if(beat == true)
        {
            playerCurrentZoom += beatVelocity * deltaTime;
            if( playerCurrentZoom >= playerMaxZoom )
            {
                playerCurrentZoom = playerMaxZoom;
                beat = false;
            }
        } else{
            playerCurrentZoom -= deltaTime;
        }

        obstaclesZoom = 1 + (1 - playerCurrentZoom) * 0.15f;


    }


    void Sample_Scene::animationDeath(float deltaTime)
    {
        if(!animationDeathFinished)
        {
            currentDeathZoomTime += deltaTime;

            currentDeathZoom = (currentDeathZoomTime / deathZoomTime ) * maxDeathZoom;

            if(currentDeathZoom >= maxDeathZoom)
            {
                currentDeathZoomTime = 0;
                currentDeathZoom = maxDeathZoom;
                animationDeathFinished = true;
            }
        }

    }

    void Sample_Scene::animationReset(float deltaTime)
    {
        timeSinceResetingLevel += deltaTime;

        currentDeathZoom = ( 1 - (timeSinceResetingLevel / resetZoomTime) ) * maxDeathZoom;

        if(currentDeathZoom <= 0)
        {
            timeSinceResetingLevel = 0;
            currentDeathZoom = 0;
            resetingLevel = false;
        }

    }

    void Sample_Scene::resetLevel()
    {
        obstaclePool->despawnAll();
        dead = false;
        obstaclePool->setObstaclesSpeed(obstacleSpeed);
        animationDeathFinished= false;
        resetingLevel = true;
    }

    void Sample_Scene::handle (basics::Event & event)
    {
        switch (event.id)
        {
            case ID(touch-started):   // El usuario toca la pantalla
            {
            }
            case ID(touch-moved):
            {
                Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };
                if(touchingBackButton(touch_location))
                {
                    pauseDeathScreen();
                }else if(animationDeathFinished)
                {
                    resetLevel();
                }

                float direction = touch_location.coordinates.x();

                if(direction > canvas_width*0.5) {playerDirection = RIGHT;}
                else {playerDirection = LEFT;}

                break;
            }

            case ID(touch-ended):       // El usuario deja de tocar la pantalla
            {
                playerDirection = NEUTRAL;

                break;
            }
        }
    }






    void Sample_Scene::load_xml ()
    {
        // Se lee el contenido del archivo:

        auto xml_file = Asset::open ("texts.xml");

        vector<byte> xml_bytes;

        xml_file->read_all (xml_bytes);

        // RapidXML espera un carÃ¡cter nulo al final, por lo que hay que aÃ±adirlo:

        xml_bytes.push_back (0);

        // Se parsea el contenido del archivo XML:

        xml_document< > document;

        document.parse< 0 > ((char *)xml_bytes.data ());

        // Se obtiene el tag raÃ­z (que deberÃ­a ser <scene>) y se llama a la funciÃ³n que lo interpretarÃ¡:

        xml_node< > * scene_node = document.first_node ();

        parse_scene_node (scene_node);
    }

    void Sample_Scene::parse_scene_node (rapidxml::xml_node< > * scene_node)
    {
        //Primero tiene que encontrar parametros de la escena como velocidad del jugador, necesario para otros calculos como
        //El tiempo entre obstaculos
        bool sceneParamFinded = false;
        // Se recorre toda la lista de nodos anidados dentro de <scene> (solo el primer nivel):

        for (xml_node<> * child = scene_node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type () == node_element)
            {
                string name(child->name ());

                //Primero mira si el primer nodo es de tipo sceneParameter
                if (!sceneParamFinded && name != "sceneParam")
                {
                    throw ("sceneParam node MUST BE FIRST");
                } else if(name == "sceneParam")
                {
                    sceneParamFinded = true;
                    parse_sceneParam_node(child);
                }


                if (name == "text")
                {
                    //parse_text_node (child);
                }
                else if(name == "obstaclePattern")
                {
                    obstaclePatterns.push_back( parse_ObstaclePattern_node(child) );
                }

                else if(name == "colorSet")
                {
                    colorSets.push_back( parse_colorSet_node( child ) );
                }
            }
        }
    }

    void Sample_Scene::parse_sceneParam_node (rapidxml::xml_node< > * sceneParam_node)
    {
        for (xml_node<> * child = sceneParam_node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type() == node_element)
            {
                string name(string(child->name ()));

                if (name == "backButtonWidth") backButtonWidth = parse_float_node(child) * canvas_width;
                if (name == "backButtonPosition") setBackButton(parse_position_node(child));

                if (name == "playerSize") playerSize = canvas_height * parse_float_node(child); //El player size depende de la altura del canvas
                if (name == "playerBorderSize") playerBorderSize = canvas_height * parse_float_node(child);
                if (name == "playerSpeed") playerSpeed = parse_float_node(child);

                if (name == "obstacleSpeed") obstacleSpeed = parse_float_node(child) * canvas_height;

                if (name == "secondsBetweenDirectionChange") secondsBetweenDirectionChange = parse_float_node(child);
                if (name == "rotationSpeed") rotationSpeed = parse_float_node(child);

                if (name == "timeBetwwenColorChange") timeBetwwenColorChange = parse_float_node(child);
                if (name == "timeOfColor") timeOfColor = parse_float_node(child);
                if (name == "timeBetweenColorChangeSections") timeBetweenColorChangeSections = parse_float_node(child);

                if (name == "playerMaxZoom") playerMaxZoom = parse_float_node(child);
                if (name == "playerMinZoom") playerMinZoom = parse_float_node(child);
                if (name == "beatVelocity") beatVelocity = parse_float_node(child);

                if (name == "deathZoomTime") deathZoomTime = parse_float_node(child);
                if (name == "maxDeathZoom") maxDeathZoom = parse_float_node(child);
                if (name == "resetZoomTime") resetZoomTime = parse_float_node(child);

            }
        }
    }


    ObstaclePattern Sample_Scene::parse_ObstaclePattern_node(rapidxml::xml_node< > * pattern__node)
    {
        ObstaclePattern obstaclePattern{};

        // Se recorren todos los tags anidados dentro de pattern guardando sus propiedades:

        for (xml_node<> * child = pattern__node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type() == node_element)
            {
                if (string(child->name ()) == "obstacleSlice")
                {
                    obstaclePattern.addObstacleSlice(parse_ObstacleSlice_node(child));
                }
            }
        }

        return obstaclePattern;

        // Una vez se conocen todas las propiedades de un punto, lo podemos crear y configurar:
    }

    ObstacleSlice Sample_Scene::parse_ObstacleSlice_node(rapidxml::xml_node< > * slice__node)
    {
        ObstacleSlice newObstacleSlice{};
        // Se recorren todos los obstacles y los va metiendo en el ObstacleSLice
        for(const rapidxml::xml_attribute<>* atribute = slice__node->first_attribute(); atribute; atribute = atribute->next_attribute() )
        {
            string atributeName(atribute->name());
            if(atributeName == "time")
            {
                //El tiempo que sale en el xml no está en "segundos" sino que 1 equivale a lo que tarda el jugador en dar 1 vuelta con su velocidad
                //Por lo tanto lo paso a segundos ahora que la velocidad del jugador ha sido definida
                float realTimeOfObstacleSlice = ( (2 * M_PI) / playerSpeed) * stof( atribute->value() );
                newObstacleSlice.setTimeOfSlice( realTimeOfObstacleSlice );
            }
        }

        for (xml_node<> * child = slice__node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type() == node_element)
            {
                if (string(child->name ()) == "obstacle")
                {
                    newObstacleSlice.addObstacle( parse_Obstacle_node(child) );
                }
            }
        }

        return newObstacleSlice;
    }

    ObstacleData Sample_Scene::parse_Obstacle_node(rapidxml::xml_node< > * obstacle__node)
    {
        float size;
        int slot;
        for(const rapidxml::xml_attribute<>* atribute = obstacle__node->first_attribute(); atribute; atribute = atribute->next_attribute() )
        {
            string atributeName(atribute->name ());

            if(atributeName == "width")
            {
                size = stof(atribute->value()) * canvas_height;
            }else if(atributeName == "slot")
            {
                slot = stoi(atribute->value());
            }

        }

        ObstacleData obstacleData{slot,size};
        return obstacleData;
    }



    ColorSet Sample_Scene::parse_colorSet_node(rapidxml::xml_node< > * colorSet_node )
    {
        Color darkSection{};
        Color clearSection{};
        Color mainColor{};

        // Se recorren todos los tags anidados dentro de colorSet guardando sus propiedades:

        for (xml_node<> * child = colorSet_node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type() == node_element)
            {
                if (string(child->name ()) == "color")
                {
                    string atributeName( child->first_attribute()->name() );

                    if(atributeName == "type")
                    {
                        string type{ child->first_attribute()->value() };

                        if(type == "darkSection" ) darkSection = parse_color_node(child);
                        if(type == "clearSection" ) clearSection = parse_color_node(child);
                        if(type == "mainColor" ) mainColor = parse_color_node(child);


                    }
                    else{
                        throw("first atribute must be type");
                    }
                }
                else{
                    throw("only <color> can be in colorSet");
                }
            }
        }

        return ColorSet{darkSection,clearSection,mainColor};
    }

    Color Sample_Scene::parse_color_node(rapidxml::xml_node< > *  color_node)
    {

        // Se asume que el primer nodo anidado dentro es un color, por lo que se toma
        // directamente su valor:

        string colorText = color_node->first_node ()->value();

        // Se crea un istringstream para extraer texto formateado de la cadena. Se asume que serán
        // tres números separados por una coma:

        istringstream reader(colorText);

        // Se extraen los valores que esperamos encontrar:

        float r, g, b;
        char  comma, comma2;

        reader >> r;
        reader >> comma;
        reader >> g;
        reader >> comma2;
        reader >> b;

        // Se crea un color y se devuelve:

        return Color{r,g,b};

    }

    float Sample_Scene::parse_float_node(rapidxml::xml_node< > * float_node)
    {
        // Se asume que el primer nodo anidado dentro es un color, por lo que se toma
        // directamente su valor:

        string colorText = float_node->first_node ()->value();

        // Se crea un istringstream para extraer texto formateado de la cadena.

        istringstream reader(colorText);

        // Se extraen los valores que esperamos encontrar:

        float value;

        reader >> value;

        // Se devuelve el float

        return value;
    }

    Point2f Sample_Scene::parse_position_node(rapidxml::xml_node< > * position_node )
    {
        // Se asume que el primer nodo anidado dentro es una posición, por lo que se toma
        // directamente su valor:

        string positionText = position_node->first_node ()->value();

        // Se crea un istringstream para extraer texto formateado de la cadena. Se asume que serán
        // tres números separados por una coma:

        istringstream reader(positionText);

        // Se extraen los valores que esperamos encontrar:

        float x, y;
        char  comma;

        reader >> x;
        reader >> comma;
        reader >> y;

        // Se crea un vector geométrico con las dos coordenadas y se devuelve:

        return Point2f {x,y};
    }

    void Sample_Scene::parse_text_node (xml_node< > * text_node)
    {

        // Se recorren todos los tags anidados dentro de <point> guardando sus propiedades:

        for (xml_node<> * child = text_node->first_node (); child; child = child->next_sibling ())
        {
            if (child->type() == node_element)
            {
                if (string(child->name ()) == "text")
                {

                }
            }
        }

        // Una vez se conocen todas las propiedades de un punto, lo podemos crear y configurar:

    }



    void Sample_Scene::loadSceneEsentials()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {

            // Se carga la textura de "Back":
            backTexture = Texture_2D::create (0, context, "backButton.png");

            // Se comprueba si la textura se ha podido cargar correctamente:

            if (backTexture)
            {
                context->add (backTexture);
                setBackButton(backButtonPosition);

                backTextureAdded = true;
            }
            //Ajustamos aspect ratio de esta escena
            adjustAspectRatio(context);

            //Una vez aspect ratio ajustado, montamos la escena don parametros que dependen de la pantalla
            sceneCenter = {canvas_width * 0.5f, canvas_height*0.5f};
            //Parametros de las secciones
            sectionSize = canvas_width;
            createSections(sectionSize);


            load_xml();
            player = new Player( sceneCenter, playerSize, playerBorderSize, playerSpeed);
            obstaclePool = new ObstaclePool(44,sections);
            obstaclePool->setObstaclesSpeed(obstacleSpeed);

            currentColorSet = colorSets[0];
            setSceneColor(currentColorSet);

            if(aspect_ratio_adjusted && backTextureAdded) state = READY;

        }
    }

}
