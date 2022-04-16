/*
 * @Author: HoGC
 * @Date: 2022-04-17 01:58:03
 * @Last Modified time: 2022-04-17 01:58:03
 */
#include "cc_log.h"

#if (defined(CONFIG_CC_LOG_ENABLE_TCPS_LOG) && CONFIG_CC_LOG_ENABLE_TCPS_LOG == 1)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/tcp.h"
#include "lwip/sockets.h"

static char *TAG = "cc_tcps_log";

struct tcp_pcb* g_tcps_pcb = NULL;
struct tcp_pcb* g_tcpc_pcb = NULL;

static QueueHandle_t g_tcps_log_queue = NULL;

static void tcps_log_put_task(void *arg){

    char* log = NULL;
    while(1){
        if(xQueueReceive(g_tcps_log_queue, &log, (TickType_t) portMAX_DELAY) == pdPASS){
            if(g_tcpc_pcb){
                err_t err;
                err = tcp_write(g_tcpc_pcb, log, strlen(log), TCP_WRITE_FLAG_COPY);
                if(err == ERR_OK){
                    tcp_output(g_tcpc_pcb);
                }
            }
            free(log);
        }
    }
}

int cc_tcps_log_put(const char *log, uint16_t len){

    int size = 0;
    char *log_buf = 0;
    
    log_buf = malloc(len+1);
    if(!log_buf){
        return 0;
    }
    strcpy(log_buf, log);
    
    if(g_tcps_log_queue){
        if(xQueueSendToBack(g_tcps_log_queue, (void*)&log_buf, (TickType_t) 0/portTICK_PERIOD_MS) == pdPASS){
            size = len;
        }
    }
    return size;
}

err_t tcps_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    if(p != NULL){
        pbuf_free(p);
    }else if(err == ERR_OK){
        tcp_close(pcb);
        g_tcpc_pcb = NULL;
    }

    return ERR_OK;
}

void tcps_err(void *arg, err_t err){

    if(g_tcpc_pcb){
        tcp_close(g_tcpc_pcb);
    }
}

err_t tcps_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    if(g_tcpc_pcb){
        tcp_abort(pcb);
        return ERR_ABRT;
    }

    g_tcpc_pcb = pcb;

    tcp_recv(g_tcpc_pcb, tcps_recv);
    tcp_err(g_tcpc_pcb, tcps_err);
    return ERR_OK;
}

int cc_tcps_log_init(uint16_t port){

    g_tcps_pcb = tcp_new();
    if(!g_tcps_pcb){
        CC_LOGE(TAG, "Cannot create pcb");
        return -1;
    }

    tcp_bind(g_tcps_pcb, INADDR_ANY, port);
    g_tcps_pcb = tcp_listen(g_tcps_pcb);
    tcp_accept(g_tcps_pcb, tcps_accept);

    g_tcps_log_queue  = xQueueCreate(512, sizeof(char*));

    if(!g_tcps_log_queue){
        CC_LOGE(TAG, "Queue create error");
        return -1;
    }

    xTaskCreate(tcps_log_put_task, "tcps_log_put", 2048, NULL, 3, NULL);

    return 0;
}
#endif