#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

class Carta {
private:
    string palo;
    string valor;

public:
    Carta(string palo, string valor) {
        this->palo = palo;
        this->valor = valor;
    }

    string getValor() {
        return valor;
    }

    int getValorNumerico() {
        if (valor == "A") {
            return 11; // El valor inicial del As es 11, se ajustará después si es necesario.
        }
        if (valor == "K" || valor == "Q" || valor == "J") {
            return 10;
        }
        return stoi(valor);
    }

    string toString() {
        return valor + " de " + palo;
    }
};

class Baraja {
private:
    vector<Carta> cartas;

public:
    Baraja() {
        string palos[] = {"Corazones", "Diamantes", "Tréboles", "Picas"};
        string valores[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

        for (string palo : palos) {
            for (string valor : valores) {
                cartas.push_back(Carta(palo, valor));
            }
        }

        // Mezclar las cartas
        srand(time(0));
        for (int i = 0; i < cartas.size(); i++) {
            int j = rand() % cartas.size();
            swap(cartas[i], cartas[j]);
        }
    }

    Carta repartirCarta() {
        Carta carta = cartas.back();
        cartas.pop_back();
        return carta;
    }
};

class Jugador {
private:
    vector<Carta> mano;
    int puntuacion;
    double saldo;
    double apuesta;

public:
    Jugador(double saldoInicial) {
        puntuacion = 0;
        saldo = saldoInicial;
        apuesta = 0;
    }

    void realizarApuesta(double cantidad) {
        if (cantidad <= saldo) {
            apuesta = cantidad;
            saldo -= cantidad;
        } else {
            cout << "Saldo insuficiente para apostar esta cantidad." << endl;
        }
    }

    void ganarApuesta() {
        saldo += apuesta * 2;
        apuesta = 0;
    }

    void empatarApuesta() {
        saldo += apuesta;
        apuesta = 0;
    }

    void recibirCarta(Carta carta) {
        mano.push_back(carta);
        puntuacion += carta.getValorNumerico();
        ajustarPorAses();
    }

    int getPuntuacion() {
        return puntuacion;
    }

    double getSaldo() {
        return saldo;
    }

    vector<Carta> getMano() {
        return mano;
    }

    void limpiarMano() {
        mano.clear();
        puntuacion = 0;
        apuesta = 0;
    }

private:
    void ajustarPorAses() {
        int numAses = 0;
        for (Carta carta : mano) {
            if (carta.getValor() == "A") {
                numAses++;
            }
        }
        while (puntuacion > 21 && numAses > 0) {
            puntuacion -= 10;
            numAses--;
        }
    }

public:
    string toString() {
        string manoStr = "Mano: ";
        for (auto carta : mano) {
            manoStr += carta.toString() + " | ";
        }
        manoStr += "Puntuación: " + to_string(puntuacion);
        return manoStr;
    }
};

int main() {
    cout << "Ingresa el saldo inicial del jugador: ";
    double saldoInicial;
    cin >> saldoInicial;

    Jugador jugador(saldoInicial);
    Jugador dealer(0);  // El dealer no tiene saldo ni apuesta.

    while (true) {
        Baraja baraja;
        cout << "Tu saldo actual es: $" << jugador.getSaldo() << endl;
        cout << "¿Cuánto deseas apostar? ";
        double apuesta;
        cin >> apuesta;
        jugador.realizarApuesta(apuesta);

        // Repartir cartas iniciales
        jugador.recibirCarta(baraja.repartirCarta());
        jugador.recibirCarta(baraja.repartirCarta());
        dealer.recibirCarta(baraja.repartirCarta());
        dealer.recibirCarta(baraja.repartirCarta());

        cout << "\nTus cartas: ";
        for (auto carta : jugador.getMano()) {
            cout << carta.toString() << " | ";
        }
        cout << "Puntuación: " << jugador.getPuntuacion() << endl;
        cout << "Carta visible del dealer: " << dealer.getMano()[0].toString() << endl;

        // Turno del jugador
        while (jugador.getPuntuacion() < 21) {
            cout << "¿Quieres 'hit' (pedir carta) o 'stand' (plantarte)? ";
            string opcion;
            cin >> opcion;

            if (opcion == "hit") {
                jugador.recibirCarta(baraja.repartirCarta());
                cout << "Tus cartas: ";
                for (auto carta : jugador.getMano()) {
                    cout << carta.toString() << " | ";
                }
                cout << "Puntuación: " << jugador.getPuntuacion() << endl;
                if (jugador.getPuntuacion() > 21) {
                    cout << "Te has pasado. ¡Pierdes esta ronda!" << endl;
                    break;
                }
            } else if (opcion == "stand") {
                break;
            } else {
                cout << "Opción no válida." << endl;
            }
        }

        // Turno del dealer (si el jugador no se pasó)
        if (jugador.getPuntuacion() <= 21) {
            cout << "\nCartas del dealer: ";
            for (auto carta : dealer.getMano()) {
                cout << carta.toString() << " | ";
            }
            cout << "Puntuación: " << dealer.getPuntuacion() << endl;
            while (dealer.getPuntuacion() < 17) {
                dealer.recibirCarta(baraja.repartirCarta());
                cout << "Cartas del dealer: ";
                for (auto carta : dealer.getMano()) {
                    cout << carta.toString() << " | ";
                }
                cout << "Puntuación: " << dealer.getPuntuacion() << endl;
            }
        }

        // Determinar el ganador
        if (jugador.getPuntuacion() > 21) {
            cout << "Perdiste la apuesta de $" << apuesta << endl;
        } else if (dealer.getPuntuacion() > 21 || jugador.getPuntuacion() > dealer.getPuntuacion()) {
            cout << "¡Ganaste! Has ganado $" << apuesta << endl;
            jugador.ganarApuesta();
        } else if (jugador.getPuntuacion() == dealer.getPuntuacion()) {
            cout << "Es un empate. Recuperas tu apuesta." << endl;
            jugador.empatarApuesta();
        } else {
            cout << "El dealer gana. Pierdes tu apuesta de $" << apuesta << endl;
        }

        cout << "Tu saldo actual es: $" << jugador.getSaldo() << endl;

        // Limpiar manos
        jugador.limpiarMano();
        dealer.limpiarMano();

        // Verificar si el jugador desea seguir jugando
        cout << "\n¿Quieres jugar otra ronda? (sí/no): ";
        string respuesta;
        cin >> respuesta;
        if (respuesta != "sí") {
            cout << "Gracias por jugar. Tu saldo final es: $" << jugador.getSaldo() << endl;
            break;
        }
    }

    return 0;
}
