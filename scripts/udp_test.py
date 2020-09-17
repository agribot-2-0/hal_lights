import socket
import select

TASK_TYPE_INIT_PORT = 0
TASK_TYPE_WRITE_DIO = 1
TASK_TYPE_WRITE_INDEX = 2
TASK_TYPE_EXIT = 0

MAX_PORT_NUMBER = 16
IO_LINK_REVISION_ID = 17
PROTOCOL_REVISION = 2

PROTOCOL_COMMAND_WRITE_TO_INDEX = 3
PROTOCOL_COMMAND_WRITE_DIO = 11
PROTOCOL_COMMAND_INIT_PORT = 15

INIT_FRAME_LENGTH = 36
INIT_RESULT_LENGTH_BYTE = 0
PORT_TYPE_IO_LINK = 12
PORT_TYPE_DIO = 4

WRITE_DIO_FRAME_LENGTH = 6
PORT_HIGH_STATE = 1
PORT_LOW_STATE = 0

INDEX_BASE_MSG_LENGTH = 9
INDEX_RESULT_ACK_BYTE = 3
INDEX_ACK_SUCCES = 8
INDEX_ERROR_BYTE = 5
INDEX_ADD_ERROR_BYTE = 6


def write_index_subindex_port_frame(port_num, index, sub_ind, data_len,
                                    data_array):
    message = bytearray()
    message.append(INDEX_BASE_MSG_LENGTH + data_len)  #0
    message.append(0x00)  #1
    message.append(PROTOCOL_REVISION)  #2
    message.append(PROTOCOL_COMMAND_WRITE_TO_INDEX)  #3
    if port_num < MAX_PORT_NUMBER:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    message.append(index[0])  #5 index low byte
    message.append(index[1])  #6 index high byte
    message.append(sub_ind)  #7
    message.append(data_len)  #8
    for byte in data_array:
        message.append(byte)  #9-n
    return True, message


def write_dio_port_frame(port_num, port_state):
    message = bytearray()
    message.append(WRITE_DIO_FRAME_LENGTH)  #0
    message.append(0x00)  #1
    message.append(PROTOCOL_REVISION)  #2
    message.append(PROTOCOL_COMMAND_WRITE_DIO)  #3
    if port_num < MAX_PORT_NUMBER:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    if port_state > PORT_LOW_STATE:
        message.append(PORT_HIGH_STATE)  #5
    else:
        message.append(PORT_LOW_STATE)  #5
    return True, message


def create_port_initialization_frame(port_num, port_typ, vendor_id,
                                     product_id):
    message = bytearray()
    message.append(INIT_FRAME_LENGTH)  #0
    message.append(0x00)  #1
    message.append(PROTOCOL_REVISION)  #2
    message.append(PROTOCOL_COMMAND_INIT_PORT)  #3
    if port_num < MAX_PORT_NUMBER:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    message.append(0x00)  #5
    if port_typ == 0:
        message.append(PORT_TYPE_IO_LINK)  #6
    elif port_typ == 1:
        message.append(PORT_TYPE_DIO)  #6
    else:
        print "Wrong port type, possible types are 0 - IO-link, 1 - DIO"
        return False, message
    message.append(IO_LINK_REVISION_ID)  #7
    message.append(0x00)  #8
    message.append(0x00)  #9
    message.append(0x00)  #10
    message.append(0x00)  #11
    message.append(0x00)  #12
    message.append(vendor_id[0])  #13 vendor ID
    message.append(vendor_id[1])  #14 vendor ID
    message.append(product_id[0])  #15 product ID
    message.append(product_id[1])  #16 product ID
    message.append(0x00)  #17 serial number highest byte
    message.append(0x00)  #18
    message.append(0x00)  #19
    message.append(0x00)  #20
    message.append(0x00)  #21
    message.append(0x00)  #22
    message.append(0x00)  #23
    message.append(0x00)  #24
    message.append(0x00)  #25
    message.append(0x00)  #26
    message.append(0x00)  #27
    message.append(0x00)  #28
    message.append(0x00)  #29
    message.append(0x00)  #30
    message.append(0x00)  #31
    message.append(0x00)  #32
    message.append(0x00)  #33 serial number lowest byte
    message.append(0x80)  #34 number of max input bytes (255 max)
    message.append(0x80)  #35
    return True, message


TIMEOUT_IN_SEC = 2
UDP_IP = "192.168.3.31"
UDP_PORT = 1999
UDP_PORT_IN = 2000
DEFAULT_VENDOR_ID = b'\x02\x21'
DEFAULT_PRODUCT_ID = b'\x00\x9A'
SOCK_OUT = socket.socket(
    socket.AF_INET,  # Internet
    socket.SOCK_DGRAM)  # UDP
SOCK_INPUT = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
SOCK_INPUT.bind(("", UDP_PORT_IN))
print "UDP target IP: %s" % UDP_IP
print "UDP target port: %s" % UDP_PORT
while True:
    TASK_TYPE = input(
        "\nChoose function: \n0 - init port,\n1 - turn DO LOW/HIGH,\n2 - Write to index/subidnex"
        + "\n3 - EXIT\nCOMMAND: ")
    if TASK_TYPE == TASK_TYPE_INIT_PORT:
        PORT_NUMBER = input("Enter port NUMBER (0 to 15): ")
        PORT_TYPE = input("Enter port TYPE (for IO-link -> 0, for DI -> 1): ")
        RESULT, MSG = create_port_initialization_frame(PORT_NUMBER, PORT_TYPE,
                                                       DEFAULT_VENDOR_ID,
                                                       DEFAULT_PRODUCT_ID)
        if RESULT:
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA, ADRESS = SOCK_INPUT.recvfrom(
                    1024)  # buffer size is 1024 bytes
                if ord(DATA[ord(DATA[INIT_RESULT_LENGTH_BYTE]) - 1]) == 0:
                    print "Succes, port initialized"
                else:
                    print "Failure, Balluf Hub error code: %d" % ord(
                        DATA[0][ord(DATA[0][INIT_RESULT_LENGTH_BYTE]) - 1])
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and port was not initialized"
    elif TASK_TYPE == TASK_TYPE_WRITE_DIO:
        PORT_NUMBER = input("Enter port NUMBER (0 to 15): ")
        PORT_SET = input("Enter port value (HIGH -> 1, LOW -> 0): ")
        RESULT, MSG = write_dio_port_frame(PORT_NUMBER, PORT_SET)
        if RESULT:
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA, ADRESS = SOCK_INPUT.recvfrom(
                    1024)  # buffer size is 1024 bytes
                if ord(DATA[ord(DATA[INIT_RESULT_LENGTH_BYTE]) - 1]) == 0:
                    print "Succes, value set on port %d" % PORT_NUMBER
                else:
                    print "Failure, Balluf Hub error code: %d" % ord(
                        DATA[ord(DATA[INIT_RESULT_LENGTH_BYTE]) - 1])
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and nothing set on port"
    elif TASK_TYPE == TASK_TYPE_WRITE_INDEX:
        PORT_NUMBER = input("Enter port NUMBER (0 to 15): ")
        INDEX_H = input("Enter index high byte: ")
        INDEX_L = input("Enter index low byte: ")
        INDEX_TUPPLE = [INDEX_L, INDEX_H]
        SUBINDEX = input("Enter subindex byte: ")
        # creating an empty data list
        DATA_LST = []
        # number of elemetns as input
        N = int(input("Enter number of data bytes : "))
        # iterating till the range
        for i in range(0, N):
            ele = int(input())
            DATA_LST.append(ele)  # adding the element
        RESULT, MSG = write_index_subindex_port_frame(PORT_NUMBER,
                                                      INDEX_TUPPLE, SUBINDEX,
                                                      N, DATA_LST)
        if RESULT:
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA, ADRESS = SOCK_INPUT.recvfrom(
                    1024)  # buffer size is 1024 bytes
                print(DATA)
                if ord(DATA[INDEX_RESULT_ACK_BYTE]) == INDEX_ACK_SUCCES:
                    print "Succes, value wrote to index/subindex on port %d" % PORT_NUMBER
                else:
                    print "Failure, device error code: %d additional error code: %d" % \
                        (ord(DATA[INDEX_ERROR_BYTE]), ord(DATA[INDEX_ADD_ERROR_BYTE]))
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and nothing set on port"
    elif TASK_TYPE == 3:
        break
    else:
        print "Wrong command"
