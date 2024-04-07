#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

using namespace std;

struct State {
    int fil;
    int col;
    Orientacion brujula;
};

class ComportamientoJugador : public Comportamiento {

public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        last_action = actIDLE;
        
        current_state.fil = size;
        current_state.col = size;
        current_state.brujula = norte;

        bikini = false;
        zapatillas = false;
        bien_situado = false;
        tiempo_sin_giros = 0;
        giros_consecutivos = 0;
        this->size = size;

        //El tamaño es size*2 y empezamos en la posición size, size, para evitar que se salga del mapa auxiliar.
        mapaAux = vector < vector < unsigned char >> (2 * size, vector<unsigned char>(2 * size, '?'));

        //Inicializamos el mapa con paredes en los bordes, las cuales sabemos según el guión que son 3 filas y columnas en los bordes.
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < size; j++) {
                mapaResultado[i][j] = 'P';
                mapaResultado[size - 3 + i][j] = 'P';
            }

        for (int i = 0; i < size; i++)
            for (int j = 0; j < 3; j++) {
                mapaResultado[i][j] = 'P';
                mapaResultado[i][size - 3 + j] = 'P';
            }
    }

    ComportamientoJugador(const ComportamientoJugador &comport) : Comportamiento(comport) {}

    ~ComportamientoJugador() {}

    Action think(Sensores sensores);

    int interact(Action accion, int valor);

private:
    /**
    *@brief Vectores para almacenar las posiciones de los sensores en las orientaciones norte y noreste (a partir de ellas se pueden saber todas)
    */
    const vector <pair<int, int>> sensores_norte = {{0,  0},
                                                    {-1, -1},
                                                    {-1, 0},
                                                    {-1, 1},
                                                    {-2, -2},
                                                    {-2, -1},
                                                    {-2, 0},
                                                    {-2, 1},
                                                    {-2, 2},
                                                    {-3, -3},
                                                    {-3, -2},
                                                    {-3, -1},
                                                    {-3, 0},
                                                    {-3, 1},
                                                    {-3, 2},
                                                    {-3, 3}};
    const vector <pair<int, int>> sensores_noreste = {{0,  0},
                                                      {-1, 0},
                                                      {-1, 1},
                                                      {0,  1},
                                                      {-2, 0},
                                                      {-2, 1},
                                                      {-2, 2},
                                                      {-1, 2},
                                                      {0,  2},
                                                      {-3, 0},
                                                      {-3, 1},
                                                      {-3, 2},
                                                      {-3, 3},
                                                      {-2, 3},
                                                      {-1, 3},
                                                      {0,  3}};

    Action last_action;
    State current_state;
    bool bikini, zapatillas, bien_situado;
    int tiempo_sin_giros, giros_consecutivos, size;
    /**
     * @brief Mapa que se va a ir actualizando con las percepciones del agente mientras no se oriente, para una vez orientado que se copien a su posición correspondiente.
     */
    vector <vector<unsigned char>> mapaAux;

    /**
     * @brief Este método guarda en el @param mapa los valores que se perciben por los sensores
     * @param sensores Sensores de los que obtener la información
     * @param mapa Mapa en el que guardar la información
    */
    void guardar_mapa(const Sensores& sensores, vector <vector<unsigned char>> &mapa);
    /**
     * @brief Este método actualiza la posición del agente en función de su última acción.
     * @param sensores Sensores del agente, utilizados para comprobar que no se colisionó.
    */
    void actualizar_movimiento_anterior(const Sensores& sensores);
    /**
     * @brief Este método actualiza la orientación del agente cuando se encuentra en una casilla de orientación, y guarda el mapa auxiliar en el mapa del resultado.
     * @param sensores Sensores del agente, utilizados para asignar la posición y orientación correctas.
    */
    void situarse(const Sensores& sensores);
    /**
     * @brief Este método copia el mapa auxiliar en el mapa del resultado cuando se orienta este
     * (podria evitarse e introducir el codigo en el metodo orientarse sin embargo pienso que así se comprende mejor el codigo)
     * @param sensores Sensores del agente, usados para calcular en que posición debe ir cada casilla auxiliar.
    */
    void copiar_mapa_aux(const Sensores& sensores);
    /**
     * @brief Este método rota el mapa auxiliar en función de la orientación del agente, usado en el nivel 3 para rotar la matriz auxilar.
    */
    void rotar_mapa_aux(const Sensores& sensores);
    /**
     * @brief Este método es de los más importantes, pues es el que decide la acción a realizar en función de las percepciones del agente, lo que lo convierte en un agente reactivo.
     * @param sensores Sensores del agente, usados para decidir la acción a realizar.
     * @return Acción a realizar.
    */
    Action elegir_accion(const Sensores& sensores);
    /**
     * @brief A partir de este método se resetean todas las variables cuando se produce un reinicio.
    */
    void reseteo_variables();
    /**
     * @brief Este método busca en los sensores si hay un objeto de los introducidos en el vector de casillas.
     * @param casillas Vector de objetos a buscar.
     * @param sensores Sensores del agente.
     * @return Par de enteros con la posición del objeto en los sensores, (fila y columna).
    */
    pair<unsigned int, unsigned int> buscarCasilla(const vector<unsigned char> &casillas, const Sensores& sensores) const;
    /**
     * @brief Esta función devuelve la acción a seguir para ir hacia un objetivo.
     * @param sensores Sensores del agente.
     * @param objetivo Par de enteros con la posición del objetivo.
     * @return Acción a seguir.
    */
    Action seguir_objetivo(const Sensores &sensores, const pair<unsigned int, unsigned int>& objetivo) const;
    /**
     * @brief Este método devuelve las coordenadas de un punto en función de su posición en el vector de sensores.terreno.
     * @param pos Indice del vector.
     * @return Par de enteros con las coordenadas (fila y columna).
    */
    pair<unsigned int, unsigned int> obtener_coordenadas(const unsigned int &pos) const;
};

#endif
