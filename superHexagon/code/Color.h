//
// Created by Darío on 22/01/2022.
//

#ifndef BASICS_ACCELEROMETER_EXAMPLE_COLOR_H
#define BASICS_ACCELEROMETER_EXAMPLE_COLOR_H

namespace hexagonGame {

    using namespace std;

    /**
    * @brief clase simple para almacenamiento de color
    */
    struct Color {

    private:
        float r;
        float g;
        float b;


    public:
        /**
        * @brief constructor de colores siendo 0 el minimo y 1 el maximo valor
         * @param r: valor de rojo
         * @param g: valor de verde
         * @param b: valor de azul
        */
        Color(float r, float g, float b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        /**
        * @brief por defecto el color blanco
        */
        Color() : Color(1,1,1){}

    public:

        /**
        * @return devuelve el valor de color del rojo
        */
        const float & red() const { return r; }
        /**
        * @return devuelve el valor de color del verde
        */
        const float & green()const { return g; }
        /**
        * @return devuelve el valor de color del azul
        */
        const float & blue()const { return b; }

    public:
        /**
        * @brief iguala los valores de un color a este
        */
        Color & operator = (const Color & other){

            r = other.r;
            g = other.g;
            b = other.b;

            return *this;

        }

    public:

        /**
        * @brief devuelve un color interpolado entre color1 y color2
         * @param color1: el color base
         * @param color2: el color al que intenta acercarse
         * @param proportion: en caso de ser "0", devuelve color1 y en caso de "1" devuelve color 2, valores intermedios devuelven colores intermedios
         * @return color interpolado entre color1 y color 2 dependiendo de la proporcion
        */
        static Color lerp(const Color & color1, const Color & color2, const float proportion)
        {
            float newR = color1.r + (color2.r-color1.r) * proportion;
            float newG = color1.g + (color2.g-color1.g) * proportion;
            float newB = color1.b + (color2.b-color1.b) * proportion;

            return Color(newR, newG, newB);
        }


    };


    /**
     * @brief almacena 3 colores, los necesarios para montar el escenario
    */
    class ColorSet
    {
    private:
        Color darkSectionColor{1,1,1};
        Color clearSectionColor{1,1,1};
        Color mainColor{1,1,1};

    public:
        /**
        * @brief constructor de colorSet
        * @param darkSectionColor: colo11 de las secciones e interior del personaje
        * @param clearSectionColor: color2 de las secciones
        * @param mainColor: color de obstaculos y borde de personaje
        */
        ColorSet(Color darkSectionColor, Color clearSectionColor, Color mainColor)
        {
            this->darkSectionColor = darkSectionColor;
            this->clearSectionColor = clearSectionColor;
            this->mainColor = mainColor;
        }

        /**
        * @brief constructor de colorSet que hace que todos sus colores sean blancos por defecto
        */
        ColorSet() : ColorSet(Color{},Color{},Color{}) {}

    public:
        /**
        * @brief interpolacion lineal de los colores de cada tipo de un colorSet entre los colores de cada tipo
         * @param colorSet1: el colorSet base
         * @param colorSet2: el colorSet al que intenta acercarse
         * @param proportion: en caso de ser "0", devuelve colorSet1 y en caso de "1" devuelve colorSet2, valores intermedios devuelven colores intermedios
         * @return ColorSet interpolado entre colorSet1 y colorSet2 dependiendo de la proporcion
        */
        static ColorSet lerp(ColorSet & colorSet1, ColorSet & colorSet2, float proportion)
        {
            Color darkSectionColor = Color::lerp(colorSet1.getDarkSectionColor(), colorSet2.getDarkSectionColor(), proportion );
            Color clearSectionColor = Color::lerp(colorSet1.getClearSectionColor(), colorSet2.getClearSectionColor(), proportion );
            Color mainColor = Color::lerp(colorSet1.getMainColor(), colorSet2.getMainColor(), proportion );

            return ColorSet{darkSectionColor, clearSectionColor, mainColor};


        }

    public:
        /**
        * @return devuelve el color de la seccion más oscura
        */
        const Color & getDarkSectionColor() const
        {
            return darkSectionColor;
        }
        /**
        * @return devuelve el color de la seccion más clara
        */
        const Color & getClearSectionColor() const
        {
            return clearSectionColor;
        }
        /**
        * @return devuelve el color de el borde del personaje y los obstaculos
        */
        const Color & getMainColor() const
        {
            return mainColor;
        }

    public:
        /**
        * @brief setea el color de la seccion más oscura
         * @param newColor: color nuevo
        */
        void setDarkSectionColor(const Color & newColor)
        {
            darkSectionColor = newColor;
        }
        /**
        * @brief setea el color de la seccion más clara
        * @param newColor: color nuevo
        */
        void setClearSectionColor(const Color & newColor)
        {
            clearSectionColor = newColor;
        }
        /**
        * @brief setea el color principal que pertenece al personaje y a los obstaculos
        * @param newColor: color nuevo
        */
        void setMainColor(const Color & newColor)
        {
            mainColor = newColor;
        }

    public:
        /**
        * @brief operador de igualdad que iguala los valores de todos los colores al especificado
         *@return retorna este color con los valores cambiados
        */
        ColorSet & operator = (const ColorSet & other){

            darkSectionColor = other.darkSectionColor;
            clearSectionColor = other.clearSectionColor;
            mainColor = other.mainColor;

            return *this;
        }


    };
}

#endif //BASICS_ACCELEROMETER_EXAMPLE_COLOR_H
