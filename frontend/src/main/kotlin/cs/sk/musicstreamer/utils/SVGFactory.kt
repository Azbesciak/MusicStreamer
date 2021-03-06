package cs.sk.musicstreamer.utils

import com.jfoenix.svg.SVGGlyph
import javafx.scene.paint.Paint

object SVGFactory {
    private val color = Paint.valueOf("#ffffff")
    val next = SVGGlyph("""M38.815,40.071c0,1.272-1.271,2.181-1.271,2.181l-32.8,22.541C2.137,66.533,0,65.252,0,61.949
           V18.191c0-3.302,2.137-4.582,4.744-2.843l32.8,22.541C37.544,37.889,38.815,38.799,38.815,40.071z M78.869,37.889l-33.98-22.541
              c-2.607-1.738-4.746-0.459-4.746,2.843v43.758c0,3.303,2.138,4.584,4.746,2.844l33.98-22.541c0,0,1.273-0.909,1.273-2.181
                 S78.869,37.889,78.869,37.889z""",
            color)

    val play = SVGGlyph("""M36.068,20.176l-29-20C6.761-0.035,6.363-0.057,6.035,0.114C5.706,0.287,5.5,0.627,5.5,0.999v40
        |  c0,0.372,0.206,0.713,0.535,0.886c0.146,0.076,0.306,0.114,0.465,0.114c0.199,0,0.397-0.06,0.568-0.177l29-20
        |    c0.271-0.187,0.432-0.494,0.432-0.823S36.338,20.363,36.068,20.176z""".trimMargin(),
            color)

    val stop = SVGGlyph("""M288,0H32C14.368,0,0,14.368,0,32v256c0,17.632,14.368,32,32,32h256c17.632,0,32-14.368,32-32V32
          C320,14.368,305.632,0,288,0z""",
            color)

}