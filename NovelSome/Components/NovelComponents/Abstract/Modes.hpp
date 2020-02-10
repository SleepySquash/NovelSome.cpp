//
//  Modes.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 09.02.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef VNComponentsModes_hpp
#define VNComponentsModes_hpp

namespace ns
{
    enum class Mode { Appear, Disapper, Exist, Deprecate };
    enum class MessageBack { No, AtAppearance, AtDisappearance, AtDeprecated };
    enum class Position { Custom, Left, CLeft, Center, CRight, Right };
}

#endif /* VNComponentsModes_hpp */
