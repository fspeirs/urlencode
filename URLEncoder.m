//
//  URLEncoder.m
//  URLEncode
//
//  Created by Fraser Speirs on 06/12/2007.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "URLEncoder.h"
#import "NSDictionary+BSJSONAdditions.h"

NSString *const URLEncoderErrorDomain = @"URLEncoderErrorDomain";
NSString *const URLEncoderErrorResponseStringKey = @"URLEncoderErrorResponseStringKey";

@interface URLEncoder (Private)
- (void)cleanUp;
@end

@implementation URLEncoder
@synthesize delegate, callbackConnection, callbackData, currentURLRequest, currentRequestIsEncode;

- (id)initWithDelegate:(id<URLEncoderDelegate>)del {
	if(self = [super init]) {
		[self setDelegate: del];
	}
	return self;
}

- (void)dealloc {
	[callbackConnection cancel];
	[callbackConnection release];
	[currentURLRequest release];
	[callbackData release];
	[super dealloc];
}

- (void)encodeURL:(NSURL *)url {
	if([self delegate] == nil)
		[NSException raise: NSInternalInconsistencyException
					format: [NSString stringWithFormat: @"Attempt to call encodeURL: for %@ without a delegate in place.", [url absoluteString]]];
	
	if([self currentURLRequest] != nil)
		[NSException raise: NSInternalInconsistencyException
					format: [NSString stringWithFormat: @"Attempt to call encodeURL: for %@ whilst an operation is in progress.", [url absoluteString]]];
	
	NSURL *requestURL = [NSURL URLWithString: [NSString stringWithFormat: @"http://urlenco.de/PostJSON.aspx?encode=%@", [url absoluteString]]];
	[self setCurrentURLRequest: [NSURLRequest requestWithURL: requestURL]];
	[self setCurrentRequestIsEncode: YES];	
	[self setCallbackConnection: [NSURLConnection connectionWithRequest: [self currentURLRequest]
															   delegate: self]];
}

- (void)decodeURL:(NSURL *)url {
	if([self delegate] == nil)
		[NSException raise: NSInternalInconsistencyException
					format: [NSString stringWithFormat: @"Attempt to call decodeURL: for %@ without a delegate in place.", [url absoluteString]]];
	
	if([self currentURLRequest] != nil)
		[NSException raise: NSInternalInconsistencyException
					format: [NSString stringWithFormat: @"Attempt to call decodeURL: for %@ whilst an operation is in progress.", [url absoluteString]]];
	
	NSURL *requestURL = [NSURL URLWithString: [NSString stringWithFormat: @"http://urlenco.de/PostJSON.aspx?decode=%@", [url absoluteString]]];
	[self setCurrentURLRequest: [NSURLRequest requestWithURL: requestURL]];
	[self setCurrentRequestIsEncode: NO];
	[self setCallbackConnection: [NSURLConnection connectionWithRequest: [self currentURLRequest]
															   delegate: self]];	
}
@end

@implementation URLEncoder (Private)
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
	
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
	if(![self callbackData])
		[self setCallbackData: [NSMutableData data]];
	[[self callbackData] appendData: data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
	NSString *responseJSONString = [[NSString alloc] initWithData: [self callbackData] encoding: NSUTF8StringEncoding];
	NSDictionary *dictionary = [NSDictionary dictionaryWithJSONString: responseJSONString];

	if(!dictionary) {
		NSError *err = [NSError errorWithDomain: URLEncoderErrorDomain
										   code: URLEncoderErrorCouldNotParseJSONResponse
									   userInfo: [NSDictionary dictionaryWithObject: responseJSONString forKey: URLEncoderErrorResponseStringKey]];
		
		if([self currentRequestIsEncode])
			[[self delegate] encoderFailedToEncodeURL: [[self currentURLRequest] URL] withError: err];
		else
			[[self delegate] encoderFailedToDecodeURL: [[self currentURLRequest] URL] withError: err];

		[self cleanUp];
		return;
	}

	// Check the dictionary is good
	if([dictionary objectForKey: @"encoded"] == nil || [dictionary objectForKey: @"url"] == nil) {
		NSError *err = [NSError errorWithDomain: URLEncoderErrorDomain
										   code: URLEncoderErrorMalformedJSONResponse
									   userInfo: [NSDictionary dictionaryWithObject: responseJSONString forKey: URLEncoderErrorResponseStringKey]];
		
		if([self currentRequestIsEncode])
			[[self delegate] encoderFailedToEncodeURL: [[self currentURLRequest] URL] withError: err];
		else
			[[self delegate] encoderFailedToDecodeURL: [[self currentURLRequest] URL] withError: err];
		
		[self cleanUp];
		return;	
	}		
	
	if([self currentRequestIsEncode])
		[[self delegate] encoderReceivedEncodedURL: [NSURL URLWithString: [dictionary objectForKey: @"encoded"]]
									forOriginalURL: [NSURL URLWithString: [dictionary objectForKey: @"url"]]];
	else
		[[self delegate] encoderReceivedDecodedURL: [NSURL URLWithString: [dictionary objectForKey: @"url"]]
								   forShortenedURL: [NSURL URLWithString: [dictionary objectForKey: @"encoded"]]];
	
	[self cleanUp];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
	[[self delegate] encoderFailedToDecodeURL: [[self currentURLRequest] URL] withError: error];
	[self cleanUp];
}

- (void)cleanUp {
	[self setCurrentURLRequest: nil];
	[self setCallbackConnection: nil];
	[self setCallbackData: nil];
}
@end