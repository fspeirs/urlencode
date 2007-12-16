//
//  URLEconder
//  Part of URLEncode.framework
//
//  Created by Fraser Speirs on 06/12/2007.
//  Copyright 2007 Connected Flow - http://www.connectedflow.com - All rights reserved.
//  Permission to use this code:
//
//  Feel free to use this code in your software, either as-is or 
//  in a modified form. Either way, please include a credit in 
//  your software's "About" box or similar, mentioning at least 
//  Connected Flow.
//
//  Permission to redistribute this code:
//
//  You can redistribute this code, as long as you keep these 
//  comments. You can also redistribute modified versions of the 
//  code, as long as you add comments to say that you've made 
//  modifications (keeping these original comments too).
//
//  If you do use or redistribute this code, an email would be 
//  appreciated, just to let me know that people are finding my 
//  code useful. You can reach me at info@connectedflow.com
//

#import <Cocoa/Cocoa.h>

@protocol URLEncoderDelegate
/*!
 * @method     encoderReceivedEncodedURL:forOriginalURL:
 * @abstract   Invoked by URLEncoder on its delegate when a URL was successfully encoded.
 * @param      shortenedURL The shortened version of originalURL.
 * @param      originalURL The URL that was shortened.
 */
- (void)encoderReceivedEncodedURL:(NSURL *)shortenedURL forOriginalURL:(NSURL *)originalURL;

/*!
 * @method     encoderFailedToEncodeURL:withError:
 * @abstract   Invoked by URLEncoder on its delegate when a URL failed to be encoded.
 * @param      originalURL The URL that was requested to be shortened but which failed.
 * @param      error An NSError containing information about the error. See below for error codes and UserInfo keys.
 */
- (void)encoderFailedToEncodeURL:(NSURL *)originalURL withError:(NSError *)error;

/*!
 * @method     encoderReceivedDecodedURL:forShortenedURL:
 * @abstract   Invoked by URLEncoder on its delegate when a URL was successfully encoded.
 * @param      fullURL The expanded version of the given shortened URL.
 * @param      shortenedURL The short URL that was expanded.
 */
- (void)encoderReceivedDecodedURL:(NSURL *)fullURL forShortenedURL:(NSURL *)shortenedURL;

/*!
 * @method     encoderFailedToDecodeURL:withError:
 * @abstract   Invoked by URLEncoder on its delegate when a shortened URL failed to be expanded.
 * @param      shortenedURL The short URL that was requested to be expanded but which failed.
 * @param      error An NSError containing information about the error. See below for error codes and UserInfo keys.
 */
- (void)encoderFailedToDecodeURL:(NSURL *)shortenedURL withError:(NSError *)error;
@end

// Possible error codes - these are two possible values for the -code of any NSError message returned.
typedef enum {
	URLEncoderErrorCouldNotParseJSONResponse, // If the JSON couldn't be parsed at all
	URLEncoderErrorMalformedJSONResponse,     // If the JSON could be parsed but was missing an expected key
} URLEncoderErrorType;

// Keys for the UserInfo dictionary of returned NSError
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
