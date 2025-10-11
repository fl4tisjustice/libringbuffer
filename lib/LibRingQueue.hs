{-# LANGUAGE Haskell98 #-}

-- C Interop
{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE CApiFFI #-}

-- Duck typing
{-# LANGUAGE EmptyDataDecls #-}

-- Make explicit type variables work
{-# LANGUAGE TypeApplications #-}
{-# LANGUAGE ScopedTypeVariables #-}
{-# LANGUAGE AllowAmbiguousTypes #-}

module LibRingQueue (ringQueueInit, LibRingQueue.length, capacity, enqueue, pop) where

import Foreign.C.Types
import Foreign (Ptr, FunPtr, castPtr)
import Foreign.Storable (Storable, sizeOf, poke, peek)
import Foreign.ForeignPtr (ForeignPtr, mallocForeignPtr, withForeignPtr, newForeignPtr)

import Data.Functor((<&>))

data RingQueueT a
type RingQueuePtr a = Ptr (RingQueueT a)
type RingQueue a = ForeignPtr (RingQueueT a)

-- Mirror C++ exceptions
data RingQueueError = OutOfRangeError | LengthError
    deriving Show

foreign import capi "ringqueue_interface.h ringqueue_create" ringqueue_create :: CSize -> CSize -> IO (RingQueuePtr a)
foreign import capi "ringqueue_interface.h &ringqueue_free" ringqueue_free_ptr :: FunPtr (RingQueuePtr a -> IO ())

foreign import capi "ringqueue_interface.h ringqueue_enqueue" ringqueue_enqueue :: RingQueuePtr a -> Ptr () -> CSize -> IO CBool
foreign import capi "ringqueue_interface.h ringqueue_pop" ringqueue_pop :: RingQueuePtr a -> Ptr () -> CSize -> IO CBool

foreign import capi "ringqueue_interface.h ringqueue_size" ringqueue_size :: RingQueuePtr a -> IO CSize;
foreign import capi "ringqueue_interface.h ringqueue_capacity" ringqueue_capacity :: RingQueuePtr a -> IO CSize;

-- Helpers
bytes :: forall a . Storable a => CSize
bytes = fromIntegral . sizeOf @a $ undefined

bytesToCount :: forall a . Storable a => CSize -> CSize
bytesToCount = flip div $ bytes @a

-- Main functionality
ringQueueInit :: forall a . Storable a => CSize -> IO (RingQueue a)
ringQueueInit minCount = newForeignPtr ringqueue_free_ptr =<< ringqueue_create minCount (bytes @a)

length :: forall a . Storable a => RingQueuePtr a -> IO CSize
length queue = bytesToCount @a <$> ringqueue_size queue

capacity :: forall a . Storable a => RingQueuePtr a -> IO CSize
capacity queue = bytesToCount @a <$> ringqueue_capacity queue

enqueue :: forall a . Storable a => RingQueuePtr a -> a -> IO (Either Bool RingQueueError)
enqueue queue elem = mallocForeignPtr @a >>= (`withForeignPtr` \ptr ->
    do  poke ptr elem
        result <- ringqueue_enqueue queue (castPtr ptr) (bytes @a)
        return (if toEnum . fromIntegral $ result then Left True else Right LengthError)
    )

pop :: forall a . Storable a => RingQueuePtr a -> IO (Either a RingQueueError)
pop queue = mallocForeignPtr @a >>= (`withForeignPtr` \out ->
    do  result <- ringqueue_pop queue (castPtr @a out) (bytes @a)
        (if toEnum . fromIntegral $ result then Left <$> peek @a out else return . Right $ OutOfRangeError)
    )