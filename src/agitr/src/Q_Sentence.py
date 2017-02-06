#!/usr/bin/env python
import rospy
from std_msgs.msg import String
import urllib
import speech_recognition as sr
import pyttsx as pt
from nltk import word_tokenize,Text,pos_tag

pub = rospy.Publisher('command', String, queue_size=10)
rospy.init_node('Q_Sentence', anonymous=True)

engine = pt.init()
engine.setProperty('rate', 190)

# pub = rospy.Publisher('command', String, queue_size=10)
# rospy.init_node('Q_Sentence', anonymous=True)

# obtain audio from the microphone
r = sr.Recognizer()

def Speech_Text (audio):
	txt="null"
	# recognize speech using Google Speech Recognition
	try:
	    # for testing purposes, we're just using the default API key
	    # to use another API key, use `r.recognize_google(audio, key="GOOGLE_SPEECH_RECOGNITION_API_KEY")`
	    # instead of `r.recognize_google(audio)`
	    txt = r.recognize_google(audio)
	    print("Speech Recognition thinks you said :\n" + txt)
	except sr.UnknownValueError:
	    print("Speech Recognition could not understand audio")
	except sr.RequestError as e:
	    print("Please make sure you are connected to the internet; {0}".format(e))
	return txt


def construct_url (before, between, after):
	return (before+between+after)

def get_answer (query):
	fquery = query.replace(" ","+")
	url = construct_url ("https://api.wolframalpha.com/v1/spoken?i=",fquery,"&")
	response = urllib.urlopen(url)
	return (response.read())

def nlp (sentence):

	tokens = word_tokenize(sentence)
	tag = pos_tag(tokens)
	return tag

def check_command (tag):
	
	for word in tag:
		if word[0]=='right':
			# pub.publish('r')
			return 'r'
		elif  word[0]=='left':
			# pub.publish('l')
			return 'l'
		elif word[0]=='forward':
			# pub.publish('f')
			return 'f'
		elif word[0]=='backward':
			# pub.publish('b')
			return 'b'
		elif word[0]=='stop':
			# pub.publish('s')
			return 's'
	# pub.publish('n')
	return 'n'	

def talker():
	# pub = rospy.Publisher('command', String, queue_size=10)
	# rospy.init_node('Q_Sentence', anonymous=True)
    # rate = rospy.Rate(10) # 10hz
	while not rospy.is_shutdown():
		# # obtain audio from the microphone
		# r = sr.Recognizer()
		with sr.Microphone() as source:
			print("I'm ready now\n")
			audio = r.listen(source)

		txt = Speech_Text (audio)
		out = check_command( nlp(txt))
		if out == 'n':
			ans = get_answer(txt)
			print (ans)
			engine.say(ans+'!')
			engine.runAndWait()
		else:
			print out
			pub.publish(out)
			# print out
		# txt = Speech_Text ()
		# out = check_command( nlp(txt))
		

# txt = Speech_Text ()
# out = check_command( nlp(txt))

if __name__ == '__main__':
    try:
		talker()
    except rospy.ROSInterruptException:
        pass
