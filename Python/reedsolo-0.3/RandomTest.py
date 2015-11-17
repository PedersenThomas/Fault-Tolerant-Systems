import reedsolo
import random
import os
from bitarray import bitarray
import matplotlib.pyplot as plt

def ByteToBit(byteArray):
    result = bitarray()
    for byte in byteArray:
        h = '{0:08b}'.format(byte)
        arr = bitarray(h)
        result.extend(arr)
    return result

def BitToByte(bitarr):
    return bytearray(bitarr.tobytes())

def Test(messageLength, numberOfErrors):
    rs = reedsolo.RSCodec(4)
    inputData = bytearray(os.urandom(messageLength))
    encoded = rs.encode(inputData)
    bitEncoded = ByteToBit(encoded)

    # Introduce NUM_ERRORS bit flips
    indexes = random.sample(range(len(bitEncoded)), numberOfErrors)
    for index in indexes:
        bitEncoded[index] = not bitEncoded[index]
    encoded = BitToByte(bitEncoded)

    decoded = None
    try:
        decoded = rs.decode(encoded)
    except reedsolo.ReedSolomonError:
        # Too many errors for it to correct.
        return False
    
    if inputData == decoded:
        return True
    else:
        # Phantom errors. Ie. there are errors it did not catch
        return False

def RepeatedTry(messageLength, numberOfErrors, iterations=10):
    for _ in range(iterations):
        result = Test(messageLength, numberOfErrors)
        if result == True:
            return True
    return False

if __name__ == '__main__':
    data = list()
    for messageLength in range(7, 50):
        print messageLength
        for numberOfErrors in range(messageLength, 1, -1):
            result = RepeatedTry(messageLength, numberOfErrors, messageLength*2)

            if result:
                data.append( (messageLength, numberOfErrors) )
                break
                
    plt.scatter([d[0] for d in data], [d[1] for d in data], edgecolors='None', color='b')

    plt.ylabel('Number of Errors')
    plt.xlabel('Message length')
    plt.show()
    
