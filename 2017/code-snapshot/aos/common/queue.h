#ifndef AOS_COMMON_QUEUE_H_
#define AOS_COMMON_QUEUE_H_

#include <assert.h>

#include "aos/common/macros.h"
#include "aos/linux_code/ipc_lib/queue.h"
#include "aos/common/time.h"

namespace aos {

struct MessageType;

// This class is a base class for all messages sent over queues.
// All of the methods are overloaded in (generated) subclasses to do the same
// thing for the whole thing.
class Message {
 public:
  // The time that the message was sent at.
  monotonic_clock::time_point sent_time;

  Message() : sent_time(monotonic_clock::min_time) {}

  // Zeros out the time.
  void Zero();
  // Returns the size of the common fields.
  static size_t Size() { return sizeof(sent_time); }

  // Deserializes the common fields from the buffer.
  size_t Deserialize(const char *buffer);
  // Serializes the common fields into the buffer.
  size_t Serialize(char *buffer) const;

  // Populates sent_time with the current time.
  void SetTimeToNow() { sent_time = monotonic_clock::now(); }

  // Writes the contents of the message to the provided buffer.
  size_t Print(char *buffer, int length) const;

  // Compares two messages for equality, excluding their sent_time.
  bool EqualsNoTime(const Message & /*other*/) const { return true; }

  static const MessageType *GetType();
};

template <class T> class Queue;
template <class T> class MessageBuilder;
template <class T> class ScopedMessagePtr;

// A ScopedMessagePtr<> manages a queue message pointer.
// It frees it properly when the ScopedMessagePtr<> goes out of scope or gets
// sent.  By design, there is no way to get the ScopedMessagePtr to release the
// message pointer to external code.
template <class T>
class ScopedMessagePtr {
 public:
  // Returns a pointer to the message.
  // This stays valid until Send or the destructor have been called.
  const T *get() const { return msg_; }
  T *get() { return msg_; }

  const T &operator*() const {
    const T *msg = get();
    assert(msg != NULL);
    return *msg;
  }

  T &operator*() {
    T *msg = get();
    assert(msg != NULL);
    return *msg;
  }

  const T *operator->() const {
    const T *msg = get();
    assert(msg != NULL);
    return msg;
  }

  T *operator->() {
    T *msg = get();
    assert(msg != NULL);
    return msg;
  }

  operator bool() {
    return msg_ != NULL;
  }

  // Sends the message and removes our reference to it.
  // If the queue is full, over-ride the oldest message in it with our new
  // message.
  // Returns true on success, and false otherwise.
  // The message will be freed.
  bool Send();

  // Sends the message and removes our reference to it.
  // If the queue is full, block until it is no longer full.
  // Returns true on success, and false otherwise.
  // The message will be freed.
  bool SendBlocking();

  // Frees the contained message.
  ~ScopedMessagePtr() {
    reset();
  }

  // Implements a move constructor.  This only takes rvalue references
  // because we want to allow someone to say
  // ScopedMessagePtr<X> ptr = queue.MakeMessage();
  // but we don't want to allow them to then say
  // ScopedMessagePtr<X> new_ptr = ptr;
  // And, if they do actually want to move the pointer, then it will correctly
  // clear out the source so there aren't 2 pointers to the message lying
  // around.
  ScopedMessagePtr(ScopedMessagePtr<T> &&ptr)
      : queue_(ptr.queue_), msg_(ptr.msg_) {
    ptr.msg_ = NULL;
  }

 private:
  // Provide access to set_queue and the constructor for init.
  friend class aos::Queue<typename std::remove_const<T>::type>;
  // Provide access to the copy constructor for MakeWithBuilder.
  friend class aos::MessageBuilder<T>;

  // Only Queue should be able to build a message.
  ScopedMessagePtr(RawQueue *queue, T *msg)
      : queue_(queue), msg_(msg) {}

  // Sets the pointer to msg, freeing the old value if it was there.
  // This is private because nobody should be able to get a pointer to a message
  // that needs to be scoped without using the queue.
  void reset(T *msg = NULL);

  // Sets the queue that owns this message.
  void set_queue(RawQueue *queue) { queue_ = queue; }

  // The queue that the message is a part of.
  RawQueue *queue_;
  // The message or NULL.
  T *msg_;

  // Protect evil constructors.
  DISALLOW_COPY_AND_ASSIGN(ScopedMessagePtr<T>);
};

// Specializations for the Builders will be automatically generated in the .q.h
// header files with all of the handy builder methods.
template <class T>
class MessageBuilder {
 public:
  typedef T Message;
  bool Send();
};

// TODO(aschuh): Base class
// T must be a Message with the same format as the messages generated by
// the .q files.
template <class T>
class Queue {
 public:
  typedef T Message;

  explicit Queue(const char *queue_name)
      : queue_name_(queue_name), queue_(NULL), queue_msg_(NULL, NULL) {
    static_assert(shm_ok<T>::value,
                  "The provided message type can't be put in shmem.");
  }

  // Initializes the queue.  This may optionally be called to do any one time
  // work before sending information, and may be be called multiple times.
  // Init will be called when a message is sent, but this will cause sending to
  // take a different amount of time the first cycle.
  void Init();

  // Removes all internal references to shared memory so shared memory can be
  // restarted safely.  This should only be used in testing.
  void Clear();

  // Fetches the next message from the queue.
  // Returns true if there was a new message available and we successfully
  // fetched it.
  bool FetchNext();

  // Fetches the next message from the queue, waiting if necessary until there
  // is one.
  void FetchNextBlocking();

  // Fetches the last message from the queue.
  // Returns true if there was a new message available and we successfully
  // fetched it.
  bool FetchLatest();

  // Fetches the latest message from the queue, or blocks if we have already
  // fetched it until another is avilable.
  void FetchAnother();

  // Returns the age of the message.
  const monotonic_clock::duration Age() const {
    return monotonic_clock::now() - queue_msg_->sent_time;
  }

  bool IsNewerThan(const monotonic_clock::duration age) const {
    return get() != nullptr && Age() < age;
  }

  // Returns a pointer to the current message.
  // This pointer will be valid until a new message is fetched.
  const T *get() const { return queue_msg_.get(); }

  // Returns a reference to the message.
  // The message will be valid until a new message is fetched.
  const T &operator*() const {
    const T *msg = get();
    assert(msg != NULL);
    return *msg;
  }

  // Returns a pointer to the current message.
  // This pointer will be valid until a new message is fetched.
  const T *operator->() const {
    const T *msg = get();
    assert(msg != NULL);
    return msg;
  }

  // Returns a scoped_ptr containing a message.
  // GCC will optimize away the copy constructor, so this is safe.
  ScopedMessagePtr<T> MakeMessage();

  // Returns a message builder that contains a pre-allocated message.
  // This message will start out completely zeroed.
  aos::MessageBuilder<T> MakeWithBuilder();

  const char *name() const { return queue_name_; }

 private:
  const char *queue_name_;

  T *MakeRawMessage();

  // Pointer to the queue that this object fetches from.
  RawQueue *queue_;
  int index_ = 0;
  // Scoped pointer holding the latest message or NULL.
  ScopedMessagePtr<const T> queue_msg_;

  DISALLOW_COPY_AND_ASSIGN(Queue<T>);
};

// Base class for all queue groups.
class QueueGroup {
 public:
  // Constructs a queue group given its name and a unique hash of the name and
  // type.
  QueueGroup(const char *name, uint32_t hash) : name_(name), hash_(hash) {}

  // Returns the name of the queue group.
  const char *name() const { return name_.c_str(); }
  // Returns a unique hash representing this instance of the queue group.
  uint32_t hash() const { return hash_; }

 private:
  std::string name_;
  uint32_t hash_;
};

}  // namespace aos

#include "aos/linux_code/queue-tmpl.h"

#endif  // AOS_COMMON_QUEUE_H_
