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
#include <basics/Atlas>
#include <basics/Canvas>
#include <basics/Point>
#include <basics/Scene>
#include <basics/Size>
#include <basics/Timer>

#include "Sample_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

namespace hexagonGame
{

    using basics::Atlas;
    using basics::Timer;
    using basics::Canvas;
    using basics::Point2f;
    using basics::Size2f;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    using namespace basics;
    using namespace std;


    /**
    *@brief escena donde tienes opciones como "play", "help" y "creditos"
    */
    class Menu_Scene : public basics::Scene
    {

        /**
         * Representa el estado de la escena en su conjunto.
         */
        enum State
        {
            LOADING,
            READY,
            FINISHED,
            ERROR
        };

        enum Option_Id
        {
            PLAY,
            HELP,
            CREDITS
        };

        struct Option
        {
            const Atlas::Slice * slice;
            Point2f position;
            float   is_pressed;
        };

        static const unsigned number_of_options = 3;

    private:

        State    state;                                     ///< Estado de la escena.
        bool     suspended;                                 ///< true cuando la escena estÃ¡ en segundo plano y viceversa.

        unsigned canvas_width;                              ///< Ancho de la resoluciÃ³n virtual usada para dibujar.
        unsigned canvas_height;                             ///< Alto  de la resoluciÃ³n virtual usada para dibujar.

        Timer    timer;                                     ///< CronÃ³metro usado para medir intervalos de tiempo.

        Option   options[number_of_options];                ///< Datos de las opciones del menÃº

        std::unique_ptr< Atlas > atlas;                     ///< Atlas que contiene las imÃ¡genes de las opciones del menÃº

        bool menuShowing;
        bool optionsShowing;
        bool controlsShowing;
        std::shared_ptr < Texture_2D > controlsTexture;

        bool creditsShowing;
        std::shared_ptr < Texture_2D > creditsTexture;

        bool     aspect_ratio_adjusted;

    public:

        /**
        * default 1280 * 720
        */
        Menu_Scene()
        {
            state         = LOADING;
            suspended     = true;
            menuShowing = true;
            creditsShowing = false;
            optionsShowing = false;
            controlsShowing = false;
            canvas_width  = 1280;
            canvas_height =  720;
            aspect_ratio_adjusted = false;
        }

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
        bool initialize () override
        {
            for (auto & option : options)
            {
                option.is_pressed = false;
            }

            return true;
        }

        /**
         * Este metodo lo invoca Director automaticamente cuando el juego pasa a segundo plano.
         */
        void suspend () override
        {
            suspended = true;
        }

        /**
         * Este metodo lo invoca Director automaticamente cuando el juego pasa a primer plano.
         */
        void resume () override
        {
            suspended = false;
        }

        /**
         * maneja eventos en donde dependiendo de donde toques hace una cosa u otra
         */
        void handle (basics::Event & event) override
        {
            if (state == READY)                     // Se descartan los eventos cuando la escena estÃ¡ LOADING
            {
                switch (event.id)
                {
                    case ID(touch-started):         // El usuario toca la pantalla
                    case ID(touch-moved):
                    {
                        // Se determina quÃ© opciÃ³n se ha tocado:

                        Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };
                        int     option_touched = option_at (touch_location);

                        // Solo se puede tocar una opciÃ³n a la vez (para evitar selecciones mÃºltiples),
                        // por lo que solo una se considera presionada (el resto se "sueltan"):

                        for (int index = 0; index < number_of_options; ++index)
                        {
                            options[index].is_pressed = index == option_touched;
                        }

                        break;
                    }

                    case ID(touch-ended):           // El usuario deja de tocar la pantalla
                    {
                        // Se "sueltan" todas las opciones:

                        for (auto & option : options) option.is_pressed = false;

                        // Se determina quÃ© opciÃ³n se ha dejado de tocar la Ãºltima y se actÃºa como corresponda:

                        Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                        if(menuShowing)
                        {
                            if (option_at (touch_location) == PLAY)
                            {
                                director.run_scene (shared_ptr< Scene >(new Sample_Scene));
                            }

                            if (option_at (touch_location) == HELP)
                            {
                                controlsShowing = true;
                                menuShowing = false;
                                creditsShowing = false;
                            }

                            if (option_at (touch_location) == CREDITS)
                            {
                                creditsShowing = true;
                                controlsShowing = false;
                                menuShowing = false;
                            }
                        }
                        else if(controlsShowing)
                        {
                            controlsShowing = false;
                            menuShowing = true;
                        }
                        else if(creditsShowing)
                        {
                            menuShowing = true;
                            creditsShowing = false;
                        }

                        break;
                    }
                }
            }
        }

        /**
         * Este metodo se invoca automaticamente una vez por fotograma para que la escena
         * actualize su estado.
         */
        void update (float time) override
        {
            if (!suspended) if (state == LOADING)
                {
                    Graphics_Context::Accessor context = director.lock_graphics_context ();

                    if (context)
                    {
                        //Se ajusta el aspect ratio de esta escena
                        adjustAspectRatio(context);

                        // Se carga el atlas:

                        atlas.reset (new Atlas("menu-scene/main-menu.sprites", context));

                        // Si el atlas se ha podido cargar el estado es READY y, en otro caso, es ERROR:

                        state = atlas->good () ? READY : ERROR;

                        // Si el atlas estÃ¡ disponible, se inicializan los datos de las opciones del menÃº:

                        if (state == READY)
                        {
                            configure_options ();
                        }


                        // Se carga la textura de controles:
                        controlsTexture = Texture_2D::create (0, context, "controls.png");

                        // Se comprueba si la textura se ha podido cargar correctamente:

                        if (controlsTexture)
                        {
                            context->add (controlsTexture);
                        }

                        // Se carga la textura de controles:
                        creditsTexture = Texture_2D::create (1, context, "credits.png");

                        // Se comprueba si la textura se ha podido cargar correctamente:

                        if (creditsTexture)
                        {
                            context->add (creditsTexture);
                        }

                    }
                }
        }

        /**
         * Este metodo se invoca automaticamente una vez por fotograma para que la escena
         * dibuje su contenido.
         */
        void render (Graphics_Context::Accessor & context) override
        {
            if (!suspended)
            {
                // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

                Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

                // Si no se ha creado previamente, hay que crearlo una vez:

                if (!canvas)
                {
                    canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
                }

                // Si el canvas se ha podido obtener o crear, se puede dibujar con Ã©l:

                if (canvas)
                {
                    canvas->clear ();

                    if (state == READY)
                    {
                        if(menuShowing)
                        {
                            // Se dibuja el slice de cada una de las opciones del menÃº:
                            for (auto & option : options)
                            {
                                canvas->set_transform
                                        (
                                                scale_then_translate_2d
                                                        (
                                                                option.is_pressed ? 0.75f : 1.f,              // Escala de la opciÃ³n
                                                                { option.position[0], option.position[1] }      // TraslaciÃ³n
                                                        )
                                        );

                                canvas->fill_rectangle ({ 0.f, 0.f }, { option.slice->width, option.slice->height }, option.slice, CENTER | TOP);
                            }

                            // Se restablece la transformaciÃ³n aplicada a las opciones para que no afecte a
                            // dibujos posteriores realizados con el mismo canvas:

                            canvas->set_transform (Transformation2f());
                        } else if(controlsShowing)
                        {
                            if (controlsTexture)
                            {
                                showControls(canvas);
                            }
                        } else if(creditsShowing)
                        {
                            if (controlsTexture)
                            {
                                showCredits(canvas);
                            }
                        }
                    }
                }
            }
        }

    private:

        /**
         * Establece las propiedades de cada opciÃ³n si se ha podido cargar el atlas.
         */
        void configure_options ()
        {
            // Se asigna un slice del atlas a cada opciÃ³n del menÃº segÃºn su ID:

            options[PLAY   ].slice = atlas->get_slice (ID(play)   );
            options[HELP   ].slice = atlas->get_slice (ID(help)   );
            options[CREDITS].slice = atlas->get_slice (ID(credits));

            // Se calcula la altura total del menÃº:

            float menu_height = 0;

            for (auto & option : options) menu_height += option.slice->height;

            // Se calcula la posiciÃ³n del borde superior del menÃº en su conjunto de modo que
            // quede centrado verticalmente:

            float option_top = canvas_height / 2.f + menu_height / 2.f;

            // Se establece la posiciÃ³n del borde superior de cada opciÃ³n:

            for (unsigned index = 0; index < number_of_options; ++index)
            {
                options[index].position = Point2f{ canvas_width / 2.f, option_top };

                option_top -= options[index].slice->height;
            }

            // Se restablece la presiÃ³n de cada opciÃ³n:

            initialize ();
        }

        /**
         * Devuelve el Ã­ndice de la opciÃ³n que se encuentra bajo el punto indicado.
         * @param point Punto que se usarÃ¡ para determinar quÃ© opciÃ³n tiene debajo.
         * @return Ãndice de la opciÃ³n que estÃ¡ debajo del punto o -1 si no hay alguna.
         */
        int option_at (const Point2f & point)
        {
            for (int index = 0; index < number_of_options; ++index)
            {
                const Option & option = options[index];

                if
                        (
                        point[0] > option.position[0] - option.slice->width  &&
                        point[0] < option.position[0] + option.slice->width  &&
                        point[1] > option.position[1] - option.slice->height &&
                        point[1] < option.position[1] + option.slice->height
                        )
                {
                    return index;
                }
            }

            return -1;
        }

        void showControls(Canvas * canvas)
        {
            if (controlsTexture)
            {
                canvas->fill_rectangle
                        (
                                { canvas_width * .5f, canvas_height * .5f },
                                { controlsTexture->get_width (), controlsTexture->get_height () },
                                controlsTexture. get ()
                        );
            }
        }

        void showCredits(Canvas * canvas)
        {
            if (creditsTexture)
            {
                canvas->fill_rectangle
                        (
                                { canvas_width * .5f, canvas_height * .5f },
                                { creditsTexture->get_width (), creditsTexture->get_height () },
                                creditsTexture. get ()
                        );
            }
        }

        void adjustAspectRatio(Graphics_Context::Accessor & context)
        {
            if (!aspect_ratio_adjusted)
            {

                float real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();

                canvas_width = unsigned( canvas_height * real_aspect_ratio );

                aspect_ratio_adjusted = true;
            }
        }
    };


}
