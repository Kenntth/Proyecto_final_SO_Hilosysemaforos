#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <ncurses.h>

// Coordenadas de los dos barcos
int ship1_x = 0, ship1_y = 0;
int ship2_x = 20, ship2_y = 10;

// Semáforo para controlar el cruce de los barcos
sem_t crossing;

// Función para mover el primer barco
void *moveShip1(void *arg) {
    while (1) {
        // Movimiento del barco 1 en el eje x e y
        ship1_x++;
        ship1_y++;

        // Simulación del tiempo de movimiento
        usleep(300000);
        
        // Verificar si los barcos se cruzan para activar el semáforo
        if (ship1_x == ship2_x && ship1_y == ship2_y) {
            sem_post(&crossing); // Activar el semáforo
        }
    }
}

// Función para mover el segundo barco
void *moveShip2(void *arg) {
    while (1) {
        // Movimiento del barco 2 en el eje x e y
        ship2_x--;
        ship2_y--;

        // Simulación del tiempo de movimiento
        usleep(300000);
        
        // Verificar si los barcos se cruzan para activar el semáforo
        if (ship1_x == ship2_x && ship1_y == ship2_y) {
            sem_post(&crossing); // Activar el semáforo
        }
    }
}

// Función para dibujar los barcos en la interfaz gráfica
void drawShips() {
    // Iniciar la pantalla ncurses
    initscr();
    curs_set(0); // Ocultar el cursor
    
    while (1) {
        clear(); // Limpiar la pantalla
        
        // Dibujar los barcos en sus posiciones
        mvprintw(ship1_y, ship1_x, ">");
        mvprintw(ship2_y, ship2_x, "<");
        
        refresh(); // Actualizar la pantalla
        
        usleep(10000); // Pequeña pausa para la actualización visual
    }
}

int main() {
    pthread_t tid1, tid2, tid3;
    
    // Inicialización del semáforo con valor 0 (no cruzan inicialmente)
    sem_init(&crossing, 0, 0);
    
    // Crear hilos para mover los barcos
    pthread_create(&tid1, NULL, moveShip1, NULL);
    pthread_create(&tid2, NULL, moveShip2, NULL);
    
    // Crear hilo para dibujar los barcos
    pthread_create(&tid3, NULL, (void *)drawShips, NULL);
    
    // Controlar el acceso a la zona crítica (cruce de los barcos)
    while (1) {
        sem_wait(&crossing); // Esperar a que se active el semáforo
        
        // Sección crítica: cruce de los barcos
        mvprintw(15, 0, "¡Cruce de barcos en (%d, %d)!", ship1_x, ship1_y);
        
        // Reiniciar el semáforo para evitar bloqueos
        sem_wait(&crossing);
        sem_post(&crossing);
    }
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    
    // Destruir el semáforo al finalizar
    sem_destroy(&crossing);
    
    // Restaurar la terminal al estado normal
    endwin();
    
    return 0;
}
