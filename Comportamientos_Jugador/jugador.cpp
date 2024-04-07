#include "../Comportamientos_Jugador/jugador.hpp"
#include <algorithm>

using namespace std;

void ComportamientoJugador::reseteo_variables() {
    last_action = actIDLE;

    current_state.fil = size;
    current_state.col = size;
    current_state.brujula = norte;

    bikini = false;
    zapatillas = false;
    bien_situado = false;

    tiempo_sin_giros = 0;
    giros_consecutivos = 0;
    mapaAux = vector < vector < unsigned char >> (2 * size, vector<unsigned char>(2 * size, '?'));
}

void ComportamientoJugador::situarse(const Sensores& sensores) {
    copiar_mapa_aux(sensores);
    current_state.fil = sensores.posF;
    current_state.col = sensores.posC;
    current_state.brujula = sensores.sentido;
}

void ComportamientoJugador::copiar_mapa_aux(const Sensores& sensores) {
    if (current_state.brujula != sensores.sentido)
        rotar_mapa_aux(sensores);
    //Se inicializa en 3 y se termina en size - 3 para ahorrarnos las paredes, las cuales ya conocemos previamente
    for (int i = 3; i < size - 3; i++)
        for (int j = 3; j < size - 3; j++) 
            if (mapaResultado[i][j] == '?' &&
                mapaAux[current_state.fil - sensores.posF + i][current_state.col - sensores.posC + j] != '?')
                mapaResultado[i][j] = mapaAux[current_state.fil - sensores.posF + i][current_state.col - sensores.posC + j];
        
    mapaAux.clear();
}

void ComportamientoJugador::rotar_mapa_aux(const Sensores& sensores) {
    //Rotamos el mapa auxiliar según la diferencia entre la brújula y el sensor.
    switch ((sensores.sentido - current_state.brujula + 8) % 8) {
        case 2:
            swap(current_state.fil, current_state.col);
            current_state.col = size * 2 - current_state.col - 1;
            for (int i = 0; i < size * 2; ++i)
                for (int j = i + 1; j < size * 2; ++j)
                    swap(mapaAux[i][j], mapaAux[j][i]);

            for (int i = 0; i < size * 2; ++i)
                for (int j = 0; j < size; ++j)
                    swap(mapaAux[i][j], mapaAux[i][size * 2 - 1 - j]);

            break;
        case 4:
            current_state.fil = size * 2 - current_state.fil - 1;
            current_state.col = size * 2 - current_state.col - 1;
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size * 2; j++)
                    swap(mapaAux[i][j], mapaAux[size * 2 - i - 1][size * 2 - j - 1]);

            break;
        case 6:
            swap(current_state.fil, current_state.col);
            current_state.fil = size * 2 - current_state.fil - 1;
            for (int i = 0; i < size; ++i)
                for (int j = i; j < size * 2 - i - 1; ++j) {
                    swap(mapaAux[i][j], mapaAux[size * 2 - j - 1][i]);
                    swap(mapaAux[i][j], mapaAux[size * 2 - i - 1][size * 2 - j - 1]);
                    swap(mapaAux[i][j], mapaAux[j][size * 2 - i - 1]);
                }

            break;
    }
}

int ComportamientoJugador::interact(Action accion, int valor) {
    return false;
}

void ComportamientoJugador::guardar_mapa(const Sensores& sensores, vector <vector<unsigned char>> &mapa) {
    for(unsigned int i= 0; i < sensores.terreno.size();i++)
        if(sensores.terreno[i] != '?'){
            pair<unsigned int, unsigned int> pos = obtener_coordenadas(i);
            mapa[pos.first][pos.second] = sensores.terreno[i];
        }
}

pair<unsigned int, unsigned int> ComportamientoJugador::obtener_coordenadas(const unsigned int &pos) const{
    pair<unsigned int, unsigned int> salida = {0,0};
    switch (current_state.brujula) {
        case norte:
            salida = {current_state.fil + sensores_norte[pos].first, current_state.col + sensores_norte[pos].second};
            break;
        case sur:
            salida = {current_state.fil - sensores_norte[pos].first, current_state.col - sensores_norte[pos].second};
            break;
        case este:
            salida = {current_state.fil + sensores_norte[pos].second, current_state.col - sensores_norte[pos].first};
            break;
        case oeste:
            salida = {current_state.fil - sensores_norte[pos].second, current_state.col + sensores_norte[pos].first};
            break;
        case noreste:
            salida = {current_state.fil + sensores_noreste[pos].first,
                        current_state.col + sensores_noreste[pos].second};
            break;
        case noroeste:
            salida = {current_state.fil - sensores_noreste[pos].second,
                        current_state.col + sensores_noreste[pos].first};
            break;
        case sureste:
            salida = {current_state.fil + sensores_noreste[pos].second,
                        current_state.col - sensores_noreste[pos].first};
            break;
        case suroeste:
            salida = {current_state.fil - sensores_noreste[pos].first,
                        current_state.col - sensores_noreste[pos].second};
            break;
    }
    return salida;
}

pair<unsigned int, unsigned int> ComportamientoJugador::buscarCasilla(const vector<unsigned char> &casillas, const Sensores &sensores) const{
    unsigned int buscar = 0;

    if (casillas.size() == 0 ||
        find(sensores.terreno.cbegin(), sensores.terreno.cend(), 'M') != sensores.terreno.cend())
        return {0,0};
    //Se empieza en la posición 1 ya que en la 0 nos encontramos ya.
    for (int i = 1; i < sensores.terreno.size() && buscar == 0; i++)
        for (const unsigned char &casilla: casillas)
            if (sensores.terreno[i] == casilla)
                buscar = i;

    if (buscar != 0)
        return obtener_coordenadas(buscar);
    else
        return {0,0};
}

void ComportamientoJugador::actualizar_movimiento_anterior(const Sensores& sensores) {
    if (!sensores.colision)
        switch (last_action) {
            case actWALK:
                switch (current_state.brujula) {
                    case norte:
                        current_state.fil--;
                        break;
                    case noreste:
                        current_state.fil--;
                        current_state.col++;
                        break;
                    case este:
                        current_state.col++;
                        break;
                    case sureste:
                        current_state.fil++;
                        current_state.col++;
                        break;
                    case sur:
                        current_state.fil++;
                        break;
                    case suroeste:
                        current_state.fil++;
                        current_state.col--;
                        break;
                    case oeste:
                        current_state.col--;
                        break;
                    case noroeste:
                        current_state.fil--;
                        current_state.col--;
                        break;
                    default:
                        throw ("Brujula no valida");
                }
                break;
            case actRUN:
                switch (current_state.brujula) {
                    case norte:
                        current_state.fil -= 2;
                        break;
                    case noreste:
                        current_state.fil -= 2;
                        current_state.col += 2;
                        break;
                    case este:
                        current_state.col += 2;
                        break;
                    case sureste:
                        current_state.fil += 2;
                        current_state.col += 2;
                        break;
                    case sur:
                        current_state.fil += 2;
                        break;
                    case suroeste:
                        current_state.fil += 2;
                        current_state.col -= 2;
                        break;
                    case oeste:
                        current_state.col -= 2;
                        break;
                    case noroeste:
                        current_state.fil -= 2;
                        current_state.col -= 2;
                        break;
                    default:
                        throw ("Brujula no valida");
                }
                break;
            case actTURN_SR:
                current_state.brujula = static_cast<Orientacion>((current_state.brujula + 1) % 8);
                break;
            case actTURN_L:
                current_state.brujula = static_cast<Orientacion>((current_state.brujula + 6) % 8);
                break;
            case actIDLE:
                break;
            default:
                throw ("Accion no valida");
        }
}

Action ComportamientoJugador::seguir_objetivo(const Sensores &sensores, const pair<unsigned int, unsigned int>& objetivo) const {
    bool terreno_mortal_delante = (sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P');
    bool agente_delante = (sensores.agentes[2] != '_');
    Action salida = actWALK;

    if (objetivo.first < current_state.fil)
        switch (current_state.brujula) {
            case norte:
                salida = actWALK;
                break;
            case noreste:
                salida = objetivo.second > current_state.col ? actWALK : actTURN_L;
                break;
            case este:
            case sureste:
            case sur:
                salida = actTURN_L;
                break;
            case noroeste:
                salida = objetivo.second < current_state.col ? actWALK : actTURN_SR;
                break;
            case suroeste:
            case oeste:
                salida = actTURN_SR;
                break;
        }
    else if (objetivo.first > current_state.fil)
        switch (current_state.brujula) {
            case sur:
                salida = actWALK;
                break;
            case suroeste:
                salida = objetivo.second < current_state.col ? actWALK : actTURN_L;
                break;
            case norte:
            case oeste:
            case noroeste:
                salida = actTURN_L;
                break;
            case sureste:
                salida = objetivo.second > current_state.col ? actWALK : actTURN_SR;
                break;
            case noreste:
            case este:
                salida = actTURN_SR;
                break;
        }
    else if (objetivo.second < current_state.col)
        switch (current_state.brujula) {
            case oeste:
                salida = actWALK;
                break;
            case norte:
            case noreste:
            case este:
            case noroeste:
                salida = actTURN_L;
                break;
            case sureste:
            case sur:
            case suroeste:
                salida = actTURN_SR;
                break;
        }
    else if (objetivo.second > current_state.col)
        switch (current_state.brujula) {
            case este:
                salida = actWALK;
                break;
            case norte:
            case noreste:
            case noroeste:
                salida = actTURN_SR;
                break;
            case sureste:
            case sur:
            case suroeste:
            case oeste:
                salida = actTURN_L;
                break;
        }

    if (salida == actWALK) {
        // Evitar colisiones, casillas indeseadas etc.
        if (terreno_mortal_delante || agente_delante)
            salida = rand() % 2 == 0 ? actTURN_L : actTURN_SR;
    }
    return salida;
}

Action ComportamientoJugador::elegir_accion(const Sensores& sensores) {
    //Se aleatoriza la acción inicial para evitar que el agente siempre gire hacia el mismo lado.
    Action accion = accion = rand() % 2 == 0 ? actTURN_L : actTURN_SR;
    bool terreno_dificil_delante = ((sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas));
    bool terreno_mortal_delante = (sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P');
    bool agente_delante = (sensores.agentes[2] != '_');
    bool condicion_recarga = ((sensores.bateria < 3500 && sensores.vida > 1500) || sensores.bateria < 2000);
    bool casilla_importante_delante = (sensores.terreno[2] == 'K' && !bikini || sensores.terreno[2] == 'G' ||
                                       sensores.terreno[2] == 'D' && !zapatillas || sensores.terreno[2] == 'X');

    vector<unsigned char> busqueda;
    pair<unsigned int, unsigned int> objetivo = {0, 0};

    if (!bien_situado)
        busqueda.push_back('G');
    if (!bikini)
        busqueda.push_back('K');
    if (!zapatillas)
        busqueda.push_back('D');
    if (condicion_recarga)
        busqueda.push_back('X');

    objetivo = buscarCasilla(busqueda, sensores);
    
    if (objetivo.first != 0 || objetivo.second != 0)    //Si hay objetivo a seguir se sigue
        accion = seguir_objetivo(sensores, objetivo);
    else {
        //Los primeros 4 if son para salir de murallas.
        if (sensores.terreno[5] == 'M' && sensores.terreno[11] == 'M' && sensores.terreno[1] != 'M' && current_state.brujula % 2 == 0)
                accion = seguir_objetivo(sensores, obtener_coordenadas(1));
        else if (sensores.terreno[7] == 'M' && sensores.terreno[13] == 'M' && sensores.terreno[3] != 'M' && current_state.brujula % 2 == 0)
                accion = seguir_objetivo(sensores, obtener_coordenadas(3));
        else if (sensores.terreno[3] == 'M' && sensores.terreno[7] == 'M' && sensores.terreno[13] != 'M' && sensores.terreno[13] != '?' &&
                    current_state.brujula % 2 == 0)
                accion = seguir_objetivo(sensores, obtener_coordenadas(13));
        else if (sensores.terreno[1] == 'M' && sensores.terreno[5] == 'M' && sensores.terreno[11] != 'M' && sensores.terreno[11] != '?' &&
                    current_state.brujula % 2 == 0)
                accion = seguir_objetivo(sensores, obtener_coordenadas(11));
        //Si no hay nada que lo impida se indica que corra (en las casillas donde interesa energéticamente), sino, en caso de poder se anda,
        //todo esto teniendo en cuenta que en agua y bosque solo se anda si se tiene el bikini o zapatillas respectivamente.
        else if (!terreno_dificil_delante && !terreno_mortal_delante && !agente_delante)
            if (sensores.nivel != 3 && sensores.agentes[6] == '_' && (sensores.terreno[6] != 'A' || bikini)
                && (sensores.terreno[6] != 'B' || zapatillas) && sensores.terreno[6] != 'M' &&
                sensores.terreno[6] != 'P' && !casilla_importante_delante)
                accion = actRUN;
            else
                accion = actWALK;
        if (rand() % 100 > 92)
            accion = rand() % 2 == 0 ? actTURN_L : actTURN_SR;
    }
    //Si se tiene que recargar se espera
    if (sensores.terreno[0] == 'X' && condicion_recarga)
        accion = actIDLE;
    //Esto sirve para controlar que no se gire en bucle (se debe a aparecer en bosque sin botas o en agua sin bikini)
    if (accion == actTURN_L || accion == actTURN_SR) {
        tiempo_sin_giros = 0;
        giros_consecutivos++;
    } else {
        tiempo_sin_giros++;
        giros_consecutivos = 0;
    }

    if (giros_consecutivos > 5 && !terreno_mortal_delante && !agente_delante && accion != actRUN)
        accion = actWALK;

    return accion;
}

Action ComportamientoJugador::think(Sensores sensores) {
    if (sensores.reset)
        reseteo_variables();

    actualizar_movimiento_anterior(sensores);

    Action accion = elegir_accion(sensores);

    if (sensores.terreno[0] == 'K')
        bikini = true;
    if (sensores.terreno[0] == 'D')
        zapatillas = true;

    if ((sensores.terreno[0] == 'G' || sensores.nivel == 0) && !bien_situado) {
        situarse(sensores);
        bien_situado = true;
    }

    if (bien_situado)
        guardar_mapa(sensores, mapaResultado);
    else
        guardar_mapa(sensores, mapaAux);

    last_action = accion;
    return accion;
}