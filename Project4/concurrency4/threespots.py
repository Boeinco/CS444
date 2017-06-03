import threading
import random
import time
import logging

logging.basicConfig(level=logging.INFO,
		format='(%(threadName)-10s) %(message)s',)

class Rsc(object):
	def __init__(self):
		self.lock1 = threading.Lock()
		self.lock2 = threading.Lock()
		self.lock3 = threading.Lock()
		self.free_spots = 3
		self.is_full = False
	def serve(self):
		logging.info('looking for resource...')
		while(self.try_lock() != True):
			time.sleep(1)
		logging.info('process done')
	def try_lock(self):
		#non-blocking attempt to open succeeds, then:
		if self.lock1.acquire(False) == True and self.is_full == False: 
			logging.info('process found resource 1 available')
			try:
				self.free_spots -= 1
				if self.free_spots == 0:
					self.is_full = True 
				time.sleep(5)
			finally:
				while self.is_full == False:
					pass
				self.free_spots += 1
				if self.free_spots == 3:
					self.is_full = False
				self.lock1.release()
				return True
		elif self.lock2.acquire(False) == True and self.is_full == False:
			logging.info('process found resource 2 available')
			try:
				self.free_spots -= 1
				if self.free_spots == 0:
					self.is_full = True
				time.sleep(5)
			finally:
				while self.is_full == False:
					pass
				self.free_spots += 1
				if self.free_spots == 3:
					self.is_full = False
				self.lock2.release()
				return True
		elif self.lock3.acquire(False) == True and self.is_full == False:
			logging.info('process found resource 3 available')
			try:
				self.free_spots -= 1
				if self.free_spots == 0:
					self.is_full = True
				time.sleep(5)
			finally:
				while self.is_full == False:
					pass
				self.free_spots += 1
				if self.free_spots == 3:
					self.is_full = False
				self.lock3.release()
				return True
		else:
			return False
			
		

def user(cont):
	pause = random.random()
	time.sleep(pause)
	cont.serve()

random.seed(time.time())
resource = Rsc()
for i in range(9):
	t = threading.Thread(target=user, args=(resource,))
	t.start()
