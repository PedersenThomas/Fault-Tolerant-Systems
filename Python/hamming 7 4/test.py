import hamming_codes

if __name__ == '__main__':
    encodingBits = 3
    messageLength = 3000
    numberOfErrors = 50
    msg = hamming_codes.Message(encodingBits, messageLength)
    msg.corruptMessage(numberOfErrors)
    msg.parityCheck()
    m = msg.decodeMessage()
    if (m == msg.message).all():
        print 'No Errors'
    else:
        print 'Missed %i corrupted bits!'%((msg.message!=m).sum())
