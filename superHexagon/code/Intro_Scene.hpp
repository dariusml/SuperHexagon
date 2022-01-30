/*
 * Intro_Scene
 * Copyright © 2021+ Darío Muñoz Lanuza
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 */

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Timer>

#include "Menu_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>

namespace hexagonGame
{

    using namespace basics;
    using namespace std;
    using basics::Timer;
    using basics::Canvas;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    /**
    *@brief escena inicial donde el logo de ESNE hace Fade in y luego Fade out e inicia la escena Menu
    */
    class Intro_Scene : public basics::Scene
    {

        /**
         * Representa el estado de la escena en su conjunto.
         */
        enum State
        {
            UNINITIALIZED,
            LOADING,
            FADING_IN,
            WAITING,
            FADING_OUT,
            FINISHED,
            ERROR
        };

    private:

        State    state;                                     ///< Estado de la escena.
        bool     suspended;                                 ///< true cuando la escena estÃ¡ en segundo plano y viceversa.

        unsigned canvas_width;                              ///< Ancho de la resoluciÃ³n virtual usada para dibujar.
        unsigned canvas_height;                             ///< Alto  de la resoluciÃ³n virtual usada para dibujar.

        Timer    timer;                                     ///< CronÃ³metro usado para medir intervalos de tiempo.

        float    opacity;                                   ///< Opacidad de la textura.

        std::shared_ptr < Texture_2D > logo_texture;        ///< Textura que contiene la imagen del logo.
        bool     aspect_ratio_adjusted;

    public:

        /**
        * default 1280 * 720
        */
        Intro_Scene()
        {
            state         = UNINITIALIZED;
            suspended     = true;
            canvas_width  = 1280;
            canvas_height =  720;
            aspect_ratio_adjusted = false;
        }

        /**
         * Este metodo lo llama Director para conocer la resolucion virtual con la que esta¡
         * trabajando la escena.
         * @return Tamanio en coordenadas virtuales que esta usando la escena.
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
            if (state == UNINITIALIZED)
            {
                state = LOADING;
            }
            else
            {
                timer.reset ();

                opacity = 0.f;
                state   = FADING_IN;
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
         * Este metodo se invoca automaticamente una vez por fotograma para que la escena
         * actualize su estado.
         */

        void update (float time) override
        {
            if (!suspended) switch (state)
                {
                    case LOADING:    update_loading    ();
                    break;
                    case FADING_IN:  update_fading_in  (); break;
                    case WAITING:    update_waiting    (); break;
                    case FADING_OUT: update_fading_out (); break;
                    default: break;
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

                if (!canvas && aspect_ratio_adjusted)
                {
                    canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
                }

                // Si el canvas se ha podido obtener o crear, se puede dibujar con Ã©l:

                if (canvas)
                {
                    canvas->clear ();

                    if (logo_texture)
                    {
                        canvas->set_opacity (opacity);

                        canvas->fill_rectangle
                                (
                                        { canvas_width * .5f, canvas_height * .5f },
                                        { logo_texture->get_width (), logo_texture->get_height () },
                                        logo_texture. get ()
                                );
                    }
                }
            }
        }

    private:


        void update_loading ()
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                //Se ajusta el aspect ratio, que servira para las demas escenas
                adjustAspectRatio(context);

                // Se carga la textura del logo:

                logo_texture = Texture_2D::create (0, context, "logo.png");

                // Se comprueba si la textura se ha podido cargar correctamente:

                if (logo_texture)
                {
                    context->add (logo_texture);

                    timer.reset ();

                    opacity = 0.f;
                    state   = FADING_IN;
                }
                else
                    state   = ERROR;
            }
        }

        void update_fading_in ()
        {
            float elapsed_seconds = timer.get_elapsed_seconds ();

            if (elapsed_seconds < 1.f)
            {
                opacity = elapsed_seconds;      // Se aumenta la opacidad del logo a medida que pasa el tiempo
            }
            else
            {
                timer.reset ();

                opacity = 1.f;
                state   = WAITING;
            }
        }

        void update_waiting ()
        {
            // Se esperan dos segundos sin hacer nada:

            if (timer.get_elapsed_seconds () > 2.f)
            {
                timer.reset ();

                state = FADING_OUT;
            }
        }

        void update_fading_out ()
        {
            float elapsed_seconds = timer.get_elapsed_seconds ();

            if (elapsed_seconds < .5f)
            {
                opacity = 1.f - elapsed_seconds * 2.f;      // Se reduce la opacidad de 1 a 0 en medio segundo
            }
            else
            {
                // Cuando el faceout se ha completado, se lanza la siguiente escena:

                state = FINISHED;

                director.run_scene (shared_ptr< Scene >(new Menu_Scene));
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
