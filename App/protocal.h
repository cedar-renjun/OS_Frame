#include <stdint.h>

/************************************* Support Command List **************************************/
#define CMD_PROPERTY         0x1000
#define CMD_STATUS           0x1001
#define CMD_ATTACKED         0x1002
#define CMD_EQUIMENT         0x1003
#define CMD_CONTROL          0x2000
#define CMD_PING             0x3000

/**************************************** Parameters List ****************************************/

// Protocal Version
#define PROTOCAL_HEADER      0x5A
#define PROTOCAL_VERSION     0x01

// Car ID list
#define CAR_ID_A_0           0x01
#define CAR_ID_A_1           0x02
#define CAR_ID_A_2           0x03
#define CAR_ID_A_3           0x04
#define CAR_ID_A_4           0x05
#define CAR_ID_A_5           0x06
#define CAR_ID_A_6           0x07
#define CAR_ID_A_7           0x08

#define CAR_ID_B_0           0x21
#define CAR_ID_B_1           0x22
#define CAR_ID_B_2           0x23
#define CAR_ID_B_3           0x24
#define CAR_ID_B_4           0x25
#define CAR_ID_B_5           0x26
#define CAR_ID_B_6           0x27
#define CAR_ID_B_7           0x28

// headquarter
#define HEADQUARTER_A        0x30
#define HEADQUARTER_B        0x40


// Mouse Key Press Flag
#define KEY_NO_PRESS         0x00
#define KEY_PRESS            0x01

// Weapon Type
#define WEAPON_GUN_BB        0x01
#define WEAPON_GUN_GAS       0x02
#define WEAPON_GUN_RUB       0x03
#define WEAPON_CANNON        0x04





/*--------------- Attacked Point -------------------*/

//    LF-8        F-1       RF-2
//      .         .         .
//        .       .       . 
//          .     .     . 
//            .   .   . 
//              . . . 
//    L-7. . . . . . . . . . R-3
//              . . . 
//            .   .   . 
//          .     .     . 
//        .       .       . 
//      .         .         .
//    LB-6        B-5         RB-4

#define ATTACK_POINT_NULL    0x00
#define ATTACK_POINT_F       0x01
#define ATTACK_POINT_RF      0x02
#define ATTACK_POINT_R       0x03
#define ATTACK_POINT_RB      0x04
#define ATTACK_POINT_B       0x05
#define ATTACK_POINT_LB      0x06
#define ATTACK_POINT_L       0x07
#define ATTACK_POINT_LF      0x08

// Ping error value
#define ERR_NONE             0x0000
#define ERR_RF               0x0001
#define ERR_CAR              0x0002

#pragma pack(1)
typedef struct
{
    uint8_t     flag;                              // 0x5A ,����ͷ��־
    uint8_t     length;                            // Э�������ݶΣ�SEQ_NUM+PAYLOAD)����
    uint8_t     version;                           // Э��汾�����������չ(��ǰЭ��汾�ݶ�Ϊ0x01)
    uint8_t     crc8;                              // ͷУ�飬��Headerǰ3�ֽڼ���CRC8
}HEADER;

#pragma pack(1)
typedef struct
{
    uint16_t    value;                             // ��ţ��Դ��ж������Ƿ��ж������豸�յ����ݣ�
                                                   // Ӧ����ACK��ԭ�����ش��ֶ�
}SEQ_NUM;

#pragma pack(1)
typedef struct
{
    uint16_t    crc16;                             //CRC16 ��������Э��Frame����У��
}CHECKSUM;

/*--------------------------------------- Property Frame ----------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     name[16];                          // ս�����ƣ�ASCII�ַ���0x00��β
    uint8_t     car_id;                            // ս��ID���
    uint8_t     weapon;                            // ��������
    uint16_t    blood;                             // ��Ѫ��
}CAR_INFO;

#pragma pack(1)
typedef struct
{
    HEADER    header;
    SEQ_NUM   seq_number;
    uint16_t  cmd_id;
    CAR_INFO  data;
    CHECKSUM  checksum; 
}CAR_INFO_T;

/*---------------------------------------- status Frame -----------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     power;                             // ��ǰ������0-100��
    uint8_t     bullet;                            // ��ǰ��ҩ��0-100��
    uint16_t    blood_cur;                         // ��ǰѪ��
    struct
    {
        int8_t  x;                                 // λ��X
        int8_t  y;                                 // λ��Y
    }position;
    struct
    {
        int8_t  x;                                 // ƽ��x���ٶ� (-128 - 127)
        int8_t  y;                                 // ƽ��y���ٶ� (-128 - 127)
        int8_t  r;                                 // ��ת�ٶ� (-128 - 127)
    }speed;
    uint8_t     linkquality;                       // �ŵ�����

}CAR_STATUS;

#pragma pack(1)
typedef struct
{
    HEADER      header;
    SEQ_NUM     seq_number;
    uint16_t    cmd_id;
    CAR_STATUS  data;
    CHECKSUM    checksum; 
}CAR_STATUS_T;

/*----------------------------------------- Control Frame ---------------------------------------*/

#pragma pack(1)
typedef struct
{
    int16_t  mouse_x;                               // �������X
    int16_t  mouse_y;                               // �������Y
    int16_t  mouse_z;                               // �������Z
    uint8_t  left_key_press_flag;                   // �������Ƿ���
    uint8_t  right_key_press_flag;                  // ����Ҽ��Ƿ���
    uint16_t keyboard_value;                        // ��ǰ���̰���
    uint16_t reserved;
}CONTROL_INFO;

#pragma pack(1)
typedef struct
{
    HEADER        header;
    SEQ_NUM       seq_number;
    uint16_t      cmd_id;
    CONTROL_INFO  data;
    CHECKSUM      checksum;
}CONTROL_INFO_T;

/*------------------------------------------- Ping Frame ----------------------------------------*/

#pragma pack(1)
typedef struct
{    
    uint16_t  status;
}PING;

#pragma pack(1)
typedef struct
{
    HEADER    header;
    SEQ_NUM   seq_number;
    uint16_t  cmd_id;
    PING      data;
    CHECKSUM  checksum; 
}PING_T;

/*----------------------------------------- equiment Frame --------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     type;                              // ����װ������
    int16_t     blood;                             // ����/����Ѫ����Ϣ
}EQUIMENT_INFO;

#pragma pack(1)
typedef struct
{
    HEADER        header;
    SEQ_NUM       seq_number;
    uint16_t      cmd_id;
    EQUIMENT_INFO data;
    CHECKSUM      checksum;
}EQUIMENT_INFO_T;

/*------------------------------------------ Attack Frame ---------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     weapon;                            // ������������
    uint8_t     point;                             // �����
    uint16_t    blood;                             // ʧѪ��
}ATTACKED_INFO;

#pragma pack(1)
typedef struct
{
    HEADER        header;
    SEQ_NUM       seq_number;
    uint16_t      cmd_id;
    ATTACKED_INFO data;
    CHECKSUM      checksum;
}ATTACKED_INFO_T;