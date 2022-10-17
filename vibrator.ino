/* This is the code I use to run the vibrator. It is pretty basic, just relaying 5 bytes for each of the two motors and the on/off LED. */
/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules
 Pick one up today in the adafruit shop!
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!
 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
 * This sketch demonstrate how to run both Central and Peripheral roles
 * at the same time. It will act as a relay between an central (mobile)
 * to another peripheral using bleuart service.
 * 
 * Mobile <--> DualRole <--> peripheral Ble Uart
 */
#include <bluefruit.h>

// OTA DFU service
BLEDfu bledfu;

// Peripheral uart service
BLEUart bleuart;

// Central uart client
BLEClientUart clientUart;

#define PAIRING_PIN     "000000"

#define LED_PIN 3

#define INPUT_LENGTH 5

int pins[INPUT_LENGTH] = {13,12, 11,10, 9};

bool state = false;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  int c = 0;
  for(int c = 0; c < INPUT_LENGTH; c++){
    pinMode(pins[c], OUTPUT);
  }
  Serial.begin(115200);
  //while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 Dual Role BLEUART Example");
  Serial.println("-------------------------------------\n");
  
  // Initialize Bluefruit with max concurrent connections as Peripheral = 1, Central = 1
  // SRAM usage required by SoftDevice will increase with number of connections
  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  Bluefruit.setName("Clem");

  Serial.println("Setting pairing PIN to: " PAIRING_PIN);
  Bluefruit.Security.setPIN(PAIRING_PIN); // UNCOMMENT FOR PIN

  // Callbacks for Peripheral
  Bluefruit.Periph.setConnectCallback(prph_connect_callback);
  Bluefruit.Periph.setDisconnectCallback(prph_disconnect_callback);

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(cent_connect_callback);
  Bluefruit.Central.setDisconnectCallback(cent_disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start BLE Uart Service
  bleuart.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_ENC_WITH_MITM); // UNCOMMENT FOR PIN
  bleuart.begin();
  bleuart.setRxCallback(prph_bleuart_rx_callback);
  

  // Init BLE Central Uart Serivce
  clientUart.begin();
  clientUart.setRxCallback(cent_bleuart_rx_callback);


  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Filter only accept bleuart service
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  //Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop()
{
  // do nothing, all the work is done in callback
  if (bleuart.notifyEnabled())
  {
    //bleuart.println("Testing this");
  }
  delay(10000);
}

/*------------------------------------------------------------------*/
/* Peripheral
 *------------------------------------------------------------------*/
void prph_connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Prph] Connected to ");
  Serial.println(peer_name);
}

void prph_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("[Prph] Disconnected");
}


 //, 9,7, 14,15, 16,17, 18,19];

// When we read data from the connected webpage
void prph_bleuart_rx_callback(uint16_t conn_handle)
{
  state = !state;
  digitalWrite(LED_PIN, state);
  (void) conn_handle;
  
  byte str[INPUT_LENGTH] = { 0 };
  
  bleuart.read(str, INPUT_LENGTH);

  Serial.print("[Prph] RX: ");
  for(int i = 0; i < INPUT_LENGTH; i++){
    Serial.println("Byte " + String(str[i]));
    analogWrite(pins[i], byte(str[i]));
  }
}

/*------------------------------------------------------------------*/
/* Central
 *------------------------------------------------------------------*/
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet  
  Bluefruit.Central.connect(report);
}

void cent_connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Cent] Connected to ");
  Serial.println(peer_name);

  if (clientUart.discover(conn_handle))
  {
    // Enable TXD's notify
    clientUart.enableTXD();
  }else
  {
    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }  
}

void cent_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  
  Serial.println("[Cent] Disconnected");
}

/**
 * Callback invoked when uart received data
 * @param cent_uart Reference object to the service where the data 
 * arrived. In this example it is clientUart
 */
void cent_bleuart_rx_callback(BLEClientUart& cent_uart)
{
  //state = !state;
  //digitalWrite(LED_PIN, state);
  char str[20] = { 0 };
  cent_uart.read(str, 20);
      
  Serial.print("[Cent] RX: ");
  Serial.println(str);

  if (bleuart.notifyEnabled())
  {
    // Forward data from our peripheral to Mobile
    
    
  }else
  {
    // response with no prph message
    clientUart.println("[Cent] Peripheral role not connected");
  }  
}
