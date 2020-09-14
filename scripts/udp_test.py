import socket
import select


def write_index_subindex_port_frame(port_num, index, sub_ind, data_len,
                                    data_array):
    message = bytearray()
    message.append(9 + data_len)  #0
    message.append(0x00)  #1
    message.append(0x02)  #2
    message.append(0x03)  #3
    if port_num < 16:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    message.append(index[0])  #5
    message.append(index[1])  #6
    message.append(sub_ind)  #7
    message.append(data_len)  #8
    for data_iter in range(0, data_len):
        message.append(data_array[data_iter])  #8
    return True, message


def write_dio_port_frame(port_num, port_state):
    message = bytearray()
    message.append(0x06)  #0
    message.append(0x00)  #1
    message.append(0x02)  #2
    message.append(0x0b)  #3
    if port_num < 16:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    if port_state > 0:
        message.append(0x01)  #5
    else:
        message.append(0x00)  #5
    return True, message


def create_port_initialization_frame(port_num, port_typ):
    message = bytearray()
    message.append(0x24)  #0
    message.append(0x00)  #1
    message.append(0x02)  #2
    message.append(0x0f)  #3
    if port_num < 16:
        message.append(port_num)  #4 //port 00  01
    else:
        return False, message
    message.append(0x00)  #5
    if port_typ == 0:
        message.append(0x0c)  #6 // 0c IO-link 04 DI
    elif port_typ == 1:
        message.append(0x04)  #6 // 0c IO-link 04 DI
    else:
        print "Wrong port type, possible types are 0 - IO-link, 1 - DIO"
        return False, message
    message.append(0x11)  #7
    message.append(0x00)  #8
    message.append(0x00)  #9
    message.append(0x00)  #10
    message.append(0x00)  #11 vendor ID
    message.append(0x00)  #12 vendor ID
    message.append(0x02)  #13 vendor ID
    message.append(0x21)  #14 vendor ID
    message.append(0x00)  #15 product ID
    message.append(0x9A)  #16 product ID
    message.append(0x00)  #17 serial number high byte
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
    message.append(0x00)  #33 serial number low byte
    message.append(0x80)  #34 number of max input bytes
    message.append(0x80)  #35
    return True, message


TIMEOUT_IN_SEC = 2
UDP_IP = "192.168.3.31"
UDP_PORT = 1999
UDP_PORT_IN = 2000
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
    if TASK_TYPE == 0:
        PORT_NUMBER = input("Enter port NUMBER (0 to 15): ")
        PORT_TYPE = input("Enter port TYPE (for IO-link -> 0, for DI -> 1): ")
        OUTPUT_MSG = create_port_initialization_frame(PORT_NUMBER, PORT_TYPE)
        if OUTPUT_MSG[0]:
            MSG = OUTPUT_MSG[1]
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA = SOCK_INPUT.recvfrom(1024)  # buffer size is 1024 bytes
                if ord(DATA[0][ord(DATA[0][0]) - 1]) == 0:
                    print "Succes, port initialized"
                else:
                    print "Failure, Balluf Hub error code: %d" % ord(
                        DATA[0][ord(DATA[0][0]) - 1])
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and port was not initialized"
    elif TASK_TYPE == 1:
        PORT_NUMBER = input("Enter port NUMBER (0 to 15): ")
        PORT_SET = input("Enter port value (HIGH -> 1, LOW -> 0): ")
        OUTPUT_MSG = write_dio_port_frame(PORT_NUMBER, PORT_SET)
        if OUTPUT_MSG[0]:
            MSG = OUTPUT_MSG[1]
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA = SOCK_INPUT.recvfrom(1024)  # buffer size is 1024 bytes
                if ord(DATA[0][ord(DATA[0][0]) - 1]) == 0:
                    print "Succes, value set on port %d" % PORT_NUMBER
                else:
                    print "Failure, Balluf Hub error code: %d" % ord(
                        DATA[0][ord(DATA[0][0]) - 1])
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and nothing set on port"
    elif TASK_TYPE == 2:
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
        OUTPUT_MSG = write_index_subindex_port_frame(PORT_NUMBER, INDEX_TUPPLE,
                                                     SUBINDEX, N, DATA_LST)
        if OUTPUT_MSG[0]:
            MSG = OUTPUT_MSG[1]
            SOCK_OUT.sendto(MSG, (UDP_IP, UDP_PORT))
            READY = select.select([SOCK_INPUT], [], [], TIMEOUT_IN_SEC)
            if READY[0]:
                DATA = SOCK_INPUT.recvfrom(1024)  # buffer size is 1024 bytes
                if ord(DATA[0][3]) == 8:
                    print "Succes, value wrote to index/subindex on port %d" % PORT_NUMBER
                else:
                    print "Failure, device error code: %d additional error code: %d" % \
                        (ord(DATA[0][5]), ord(DATA[0][6]))
            else:
                print "Failure, socket timeout!"
        else:
            print "Wrong input data, nothing has been sent and nothing set on port"
    elif TASK_TYPE == 3:
        break
    else:
        print "Wrong command"
