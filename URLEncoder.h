//
//  URLEncoder.h
//  URLEncode
//
//  Created by Fraser Speirs on 06/12/2007.
//  Copyright 2007 Connected Flow. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol URLEncoderDelegate
- (void)encoderReceivedEncodedURL:(NSURL *)shortenedURL forOriginalURL:(NSURL *)originalURL;
- (void)encoderFailedToEncodeURL:(NSURL *)originalURL withError:(NSError *)error;

- (void)encoderReceivedDecodedURL:(NSURL *)fullURL forShortenedURL:(NSURL *)shortenedURL;
- (void)encoderFailedToDecodeURL:(NSURL *)shortenedURL withError:(NSError *)error;
@end

// Possible error codes
typedef enum {
	URLEncoderErrorCouldNotParseJSONResponse, // If the JSON couldn't be parsed at all
	URLEncoderErrorMalformedJSONResponse,     // If the JSON could be parsed but was missing an expected key
} URLEncoderErrorType;

// Keys for the UserInfo dictionary of returned errors
extern NSString *const URLEncoderErrorDomain;            // The domain of errors that are problems with JSON.
extern NSString *const URLEncoderErrorResponseStringKey; // The key that will get you the NSString that the encoding service returned.

@interface URLEncoder : NSObject {
	id <URLEncoderDelegate> delegate;       // Delegate for callbacks
	NSURLRequest *currentURLRequest;        // The request that is currently in operation
	BOOL currentRequestIsEncode;            // If YES, the current operation is an encode operation. If NO, it's a decode. Meaningless if currentURLRequest is nil.

	// Private
	NSURLConnection *callbackConnection;
	NSMutableData *callbackData;
}

@property (readwrite, assign) id <URLEncoderDelegate> delegate;
@property (readwrite, retain) NSURLConnection *callbackConnection;
@property (readwrite, retain) NSMutableData *callbackData;
@property (readwrite, retain) NSURLRequest *currentURLRequest;
@property (readwrite) BOOL currentRequestIsEncode;

- (id)initWithDelegate:(id<URLEncoderDelegate>)del;

/*!
 * @method     encodeURL
 * @abstract   Starts an asynchronous encode of the given URL.
 * @param      url The URL to encode.
 * @discussion This method starts an encode operation on the given URL. It will result in
 *             the delegate receiving either encoderReceivedEncodedURL:forOriginalURL:
 *             or encoderFailedToEncodeURL:withError:
 *
 *             Calling this method when an existing operation is in progress will result in an NSInternalInconsistencyException.
 *             Check that currentURLRequest is nil before calling this method.
 *
 *             Calling this method without providing a delegate will result in an NSInternalInconsistencyException.
 */
- (void)encodeURL:(NSURL *)url;

/*!
 * @method     decodeURL
 * @abstract   Takes a shortened URL and decodes it.
 * @param      url The URL to encode.
 * @discussion This method starts an encode operation on the given URL. It will result in
 *             the delegate receiving either encoderReceivedDecodedURL:forShortenedURL:
 *             or encoderFailedToDecodeURL:withError:
 *
 *             Calling this method when an existing operation is in progress will result in an NSInternalInconsistencyException.
 *             Check that currentURLRequest is nil before calling this method.
 *
 *             Calling this method without providing a delegate will result in an NSInternalInconsistencyException.
 */
- (void)decodeURL:(NSURL *)url;
@end
