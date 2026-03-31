# Estado del prototipo (CST_V4)

## Resultado general

**Estado actual: Prototipo estructural (base funcional parcial).**

El firmware ya tiene una arquitectura por tareas en FreeRTOS y una UI mínima en TFT, pero todavía faltan módulos críticos para operación real del banco de vibración.

## Lo que ya está en orden

- Máquina de estados base del sistema (`BOOT`, `SELFTEST`, `IDLE`, `CONFIG`, `RUNNING`, `ERROR`, `SHUTDOWN`).
- Tareas separadas para UI, control, comunicaciones y supervisión.
- Módulo de UI con pantallas base (splash/home/config/running/error).
- Módulo de entrada (encoder + botones) implementado con debounce.
- Configuración base de PlatformIO para ESP32 + ESP-IDF + LovyanGFX.

## Lo que falta para estar completamente funcional

### 1) Integración de entradas con el sistema principal

- `input_task` está implementada, pero no se crea en `app_main`.
- Hoy la UI no recibe eventos reales de hardware si no se lanza esa tarea.

### 2) Control real del actuador/vibración

- `task_control` está en placeholder (sin PWM/DAC/driver real).
- No hay capa de driver conectada al control.

### 3) Comunicaciones serie/protocolo

- `task_comm` está en placeholder; no parsea ni ejecuta comandos.
- No existe validación/rangos/ACK-NACK del protocolo.

### 4) Módulos vacíos críticos

- Archivos de `comm`, `driver` y `error_sys` están vacíos.
- Falta implementación de seguridad y manejo de fallos.

### 5) Sincronización entre UI y estado de sistema

- UI usa estado interno propio (`current_screen`) y no refleja variables reales del `system_ctx_t`.
- Pantallas muestran valores fijos (por ejemplo frecuencia/tiempo), no datos vivos.

### 6) Self-test y seguridad incompletos

- `system_selftest()` siempre retorna OK por variable hardcodeada.
- En estado `SYS_ERROR` aún no hay paro seguro implementado.

### 7) Build reproducible en este entorno

- No fue posible compilar aquí porque faltan herramientas (`pio` e `idf.py` no están instaladas en el entorno de ejecución).

## Recomendación de cierre por fases

1. **Cablear entradas + estados** (crear `input_task`, mapear eventos a cambios de `sys.state`).
2. **Implementar driver + control** (frecuencia/amplitud reales con límites y watchdog).
3. **Implementar comunicación serie** (parser robusto, comandos START/STOP/SET, respuestas).
4. **Implementar `error_sys`** (errores latcheados, códigos, recovery controlado).
5. **Conectar UI a datos reales** (`freq_setpoint`, `run_time_ms`, error code real).
6. **Completar self-test real** (pantalla, entradas, driver, sensores, NVS).
7. **Agregar pruebas mínimas** (smoke test de estados y parser de comandos).

## Criterio de “completamente funcional” (mínimo)

- Arranque + self-test real con resultado verificable.
- Comandos de inicio/paro/configuración funcionando por UI y por serial.
- Salida de vibración controlada y estable con límites de seguridad.
- Gestión de error segura (paro, mensaje, recuperación).
- Telemetría básica (estado, frecuencia, amplitud, tiempo, error).
