import reedsolo
import random
from bitarray import bitarray

def ByteToBit(byteArray):
    result = bitarray()
    for byte in byteArray:
        h = '{0:08b}'.format(byte)
        arr = bitarray(h)
        result.extend(arr)
    return result

def BitToByte(bitarr):
    return bytearray(bitarr.tobytes())

def TestReedSolo():
    print ''
    NUM_ERRORS = 2
    
    rs = reedsolo.RSCodec(2)

    inputText = bytearray([5,7,0,108,52,66])
    encoded = rs.encode(inputText)

    bitEncoded = ByteToBit(encoded)

    # Introduce NUM_ERRORS bit flips
    indexes = random.sample(range(len(bitEncoded)), NUM_ERRORS)
    for index in indexes:
        bitEncoded[index] = not bitEncoded[index]
    encoded = BitToByte(bitEncoded)
    
    decoded = None
    try:
        decoded = rs.decode(encoded)
    except reedsolo.ReedSolomonError:
        print 'Too many errors'
        return False
    
    if inputText == decoded:
        print 'Success'
        return True
    else:
        print 'Phantom error.', len(inputText), len(encoded), indexes
        print ','.join('{:02x}'.format(x) for x in inputText)
        print ','.join('{:02x}'.format(x) for x in decoded)
        return False

if __name__ == '__main__':
    results = [TestReedSolo() for _ in range(100)]
    if all(results):
        print '---ALL OK---'
    else:
        print results
    
