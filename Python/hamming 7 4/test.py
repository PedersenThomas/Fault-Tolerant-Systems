import hamming_codes
import numpy as np
import matplotlib.pyplot as plt

def TestHamming(messageLegnth, numberOfErrors):
    encodingBits = 3
    msg = hamming_codes.Message(encodingBits, messageLength, debug=False)
    msg.corruptMessage(numberOfErrors)
    msg.parityCheck()
    m = msg.decodeMessage()
    missed = (msg.message!=m).sum()
    #if (m == msg.message).all():
    #    print 'No Errors'
    #else:
    #    print 'Missed %i corrupted bits!'%(missed)
    return (missed, len(msg.encodedMessage))

def RepeatedTry(messageLength, numberOfErrors, iterations=10):
    tmp = list()
    for _ in range(iterations):
        errors, size = TestHamming(messageLength, numberOfErrors)
        tmp.append(errors)
    return (np.min(tmp), np.max(tmp), size)

if __name__ == '__main__':
    #messageLength = 3000
    numberOfErrors = 0

    data = list()
    
    for messageLength in range(4, 30):
        print messageLength
        for numberOfErrors in range(messageLength, 1, -1):
            minErr, maxErr, encodedSize = RepeatedTry(messageLength, numberOfErrors, messageLength*10)
            if minErr == 0:
                data.append( (messageLength, numberOfErrors/float(messageLength)) )
                break
                
    # Plots the data
    
    plt.scatter([d[0] for d in data], [d[1] for d in data], edgecolors='None', color='b')
    #plt.errorbar([d[0] for d in data], [d[3] for d in data], yerr=[d[4] for d in data], marker="o", color="black")
    
    #plt.xlim(0, max(bins_mean)*1.03)
    #plt.ylim(0)
    plt.ylabel('Number of Errors')
    plt.xlabel('Message length')
    plt.show()
