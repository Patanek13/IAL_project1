/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 *
 * Parametr `packetLists` je dvousměrně vázaný seznam a jako položky obsahuje
 * jednotlivé seznamy (fronty) paketů (`QosPacketListPtr`). Pokud fronta
 * s odpovídající prioritou neexistuje, tato funkce ji alokuje a inicializuje.
 * Za jejich korektní uvolnení odpovídá volající.
 *
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek
seznamu. *
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného
seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet(DLList *packetLists, PacketPtr packet) {

  if (packet == NULL) {
    return;
  }

  DLL_First(packetLists);
  QosPacketListPtr packetList = NULL;
  // Iterate through packetLists
  while (DLL_IsActive(packetLists)) {
    long data;
    DLL_GetValue(packetLists, &data);
    // Typecast long data to work with the packet structure
    QosPacketListPtr queuePtr = (QosPacketListPtr)data;
    // When found list with existing priority just add the packet to the end
    if (queuePtr->priority == packet->priority) {
      packetList = queuePtr;
      break;
    }
    DLL_Next(packetLists);
  }

  // When packet queue with specific priority doesn't exist
  if (packetList == NULL) {
    packetList = malloc(sizeof(QosPacketList));

    // Check if malloc was successful
    if (packetList == NULL) {
      return;
    }

    packetList->priority = packet->priority;
    // Malloc for structure that holds packet pointers
    packetList->list = malloc(sizeof(DLList));

    // Check if malloc was successful
    if (packetList->list == NULL) {
      free(packetList);
      return;
    }
    
    DLL_Init(packetList->list);
    // Insert the queue of packets
    DLL_InsertLast(packetLists, (long)packetList);
  }
  
  // Check if adding packet to the list doesn't exceed the limit
  if (packetList->list->currentLength + 1 > MAX_PACKET_COUNT) {
    // When packet queue is 1, dont't remove and don't add a new one
    if (packetList->list->currentLength == 1) {
      return;
    }

    DLL_First(packetList->list);
    // Remove every second packet
    while (DLL_IsActive(packetList->list)) {
      DLL_DeleteAfter(packetList->list);
      DLL_Next(packetList->list);
    }
  }
  // Insert the packet into list
  DLL_InsertLast(packetList->list, (long)packet);
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 *
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 *
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů
 * (`QosPacketListPtr`). Parametr `outputPacketList` obsahuje ukazatele na
 * odeslané pakety (`PacketPtr`).
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList,
                  int maxPacketCount) {
  // printf("DEBUG: Starting send_packets with maxPacketCount = %d\n",
  // maxPacketCount);
  int sent = 0;
  while (sent < maxPacketCount) {
    // printf("DEBUG: Loop iteration, sent = %d\n", sent);
    QosPacketListPtr highest = NULL;
    DLL_First(packetLists);

    // printf("DEBUG: Looking for highest priority queue...\n");
    while (DLL_IsActive(packetLists)) {
      long data;
      DLL_GetValue(packetLists, &data);
      QosPacketListPtr packetQueue = (QosPacketListPtr)data;

      // printf("DEBUG: Found queue with priority %d, length %d\n",
      // packetQueue->priority, packetQueue->list->currentLength);

      if (packetQueue->list->currentLength > 0) {
        if (highest == NULL || highest->priority < packetQueue->priority) {
          // printf("DEBUG: Setting this as highest priority queue (priority
          // %d)\n", packetQueue->priority);
          highest = packetQueue;
        }
      }
      DLL_Next(packetLists);
    }
    // No packets left
    if (highest == NULL) {
      // printf("DEBUG: No more packets to send, breaking\n");
      break;
    }
    // printf("DEBUG: Sending from queue with priority %d\n",
    // highest->priority);
    //  Send packets from the highest priority
    DLL_First(highest->list);
    while (DLL_IsActive(highest->list) && sent < maxPacketCount) {
      PacketPtr packet = (PacketPtr)highest->list->activeElement->data;
      // printf("DEBUG: Sending packet with ID %d, priority %d\n", packet->id,
      // packet->priority);

      DLL_InsertLast(outputPacketList, (long)packet);
      DLL_DeleteFirst(highest->list);
      sent++;
      // printf("DEBUG: Sent count now = %d\n", sent);
      DLL_First(highest->list);
    }
  }
  // printf("DEBUG: Finished send_packets, total sent = %d\n", sent);
}

/*
//TEST:
int MAX_PACKET_COUNT = 4;
int main () {
        DLList test_queue_list;
        DLL_Init(&test_queue_list);
        Packet packet = { .id = 1, .priority = 0 };
        Packet packet2 = { .id = 2, .priority = 0 };
        Packet packet3 = { .id = 3, .priority = 0 };
        Packet packet4 = { .id = 4, .priority = 0 };
        Packet packet5 = { .id = 5, .priority = 0 };
        receive_packet(&test_queue_list, &packet);
        receive_packet(&test_queue_list, &packet2);
        receive_packet(&test_queue_list, &packet3);
        receive_packet(&test_queue_list, &packet4);
        receive_packet(&test_queue_list, &packet5);
        QosPacketListPtr packetList =
(QosPacketListPtr)test_queue_list.firstElement->data; PacketPtr storedPacket =
(PacketPtr)packetList->list->lastElement->data; printf("Packet ID: %d\n",
storedPacket->id); printf("test: %d\n", packetList->priority); printf("queue
count: %d\n", packetList->list->currentLength); DLL_Dispose(&test_queue_list);
        return 0;
}
*/